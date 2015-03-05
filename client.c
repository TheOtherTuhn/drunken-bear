#include "client.h"

struct {
    game_state *gs;
    int we_are_red;
    char sid[37];
    int turn;
} current_gs = {NULL, -1, {0}, 0};

#define N_THREADS 2

struct {
    pthread_mutex_t m;
    int count;
    enum {Generate, Wait, Exit} command;
} thread_info = {PTHREAD_MUTEX_INITIALIZER, 0};

struct {
    clock_t start;
    int running;
} stopwatch = {0, 0};

int main(int argc, char *argv[])
{
    int *gs_count = malloc(sizeof(int));
    *gs_count = 0;
    move *last_move = malloc(sizeof(move));
    last_move->type = Null;
    last_move->from = last_move-> to = 0;

    char *emil = malloc(256);
    
    printf("<protocol>");
	if(argc == 1)
		printf("<join gameType=\"swc_2015_hey_danke_fuer_den_fisch\"/>\n");
	else
		printf("<joinPrepared reservationCode=\"%s\"/>\n", argv[1]);
    fflush(stdout);

    pthread_t threads[N_THREADS];
    for(int t = 0; t < N_THREADS; t++)
        pthread_create(&threads[t], NULL, &gen_gs, NULL);

    while(parseline(last_move)) {
        thread_info.command = Wait;
        while(thread_info.count > 0) usleep(50);
        update_current_gs(*last_move);
        qempty();
        push_leaves(current_gs.gs);
        thread_info.command = Generate;
        while(get_time() < 1.9) {
            usleep(200);
        }
        thread_info.command = Wait;
        while(thread_info.count > 0) usleep(50);
        *last_move = minmax(current_gs.gs)->last_move;
        sprint_move_xml(emil, *last_move, current_gs.sid);
        printf("%s\n", emil);
        fflush(stdout);
        update_current_gs(*last_move);
        qempty();
        push_leaves(current_gs.gs);
        thread_info.command = Generate;
    }
    printf("</protocol>");
    thread_info.command = Exit;
    free(emil);
    return 0;
}

uint8_t nfromc(char c)
{
    return c - '0';
}

void parse_first_gs(void)
{
    current_gs.gs = malloc(sizeof(game_state));
    init_game_state(current_gs.gs);
    size_t size = BUFSIZE;
    char *input = malloc(size);
    char *found;
    int f = 0;
    current_gs.gs->leftR = current_gs.gs->leftB = 4;
    while(getdelim(&input, &size,'>', stdin) != -1 && f < 64) {
        start_timer();
        if((found = strstr(input, "roomId=\""))) {
            memcpy(current_gs.sid, found+8, 36);
            current_gs.sid[36] = 0;
        }
        if((found = strstr(input, "WelcomeMessage\" color=\""))) {
            current_gs.we_are_red = (found[23]=='r');
        } else if((found = strstr(input, "startPlayer=\""))) {
            current_gs.gs->b_current = (found[13]=='B');
            current_gs.gs->r_current = (found[13]=='R');
        } else if((found = strstr(input, "field fish=\""))) {
            current_gs.gs->fields[f++].fish = nfromc(found[12]);
        }
    }
    free(input);
    qpush(current_gs.gs);
    return;
}

int parseline(move *last_move)
{
    size_t size = BUFSIZE;
    char *input = malloc(size);
    int r = 0;

    if(!current_gs.gs)
        parse_first_gs();

    while(getdelim(&input, &size, '>', stdin) != -1) {
        start_timer();
        if(strstr(input, "lastMove")) {
            if(strstr(input, "NullMove")) {
                last_move->type = Null;
            } else if (strstr(input, "SetMove")) {
                last_move->type = Set;
                last_move->to   = nfromc(strstr(input, "setX=\"")[6]) << 3 \
                                | nfromc(strstr(input, "setY=\"")[6]);
            } else if(strstr(input, "RunMove")) {
                last_move->type = Run;
                last_move->from = nfromc(strstr(input, "fromX=\"")[7])<< 3 \
                                | nfromc(strstr(input, "fromY=\"")[7]);
                last_move->to   = nfromc(strstr(input, "toX=\"")[5]) << 3 \
                                | nfromc(strstr(input, "toY=\"")[5]);
            }
        } else if(strstr(input, "MoveRequest")) {
            r = 1;
            goto end;
        } else if(strstr(input, "</protocol>")) {
            goto end;
        }
    }
end:
    free(input);
    input = NULL;
    return r;
}

#include "gen_thread.fun"

game_state *minmax(game_state *gs)
{
    if(!gs->first) {
        gs->rating = rate_gs(gs);
        return gs;
    }
    game_state *cur = gs->first;
    game_state *best_gs = malloc(sizeof(game_state));
    init_game_state(best_gs);
    best_gs->rating = (current_gs.we_are_red && gs->r_current)? INT_MIN : INT_MAX; 
    while((cur = cur->next)) {
        if(current_gs.we_are_red && gs->r_current) {
            best_gs = (best_gs->rating < minmax(cur)->rating)? cur : best_gs;
        } else {
            best_gs = (best_gs->rating > minmax(cur)->rating)? cur : best_gs;
        }
    }
    return best_gs;
}

void push_leaves(game_state *gs) 
{
    if(!gs) return;
    if(!gs->first) {
        qpush(gs);
        return;
    }
    push_leaves(gs->next);
    push_leaves(gs->first);
    return;
}
void update_current_gs(move played_move)
{
    game_state *cur = current_gs.gs->first;
    do {
        if(!moveequ(cur->last_move, played_move)) {
            free_branch(cur->first);
            if(cur->next) {
                cur = cur->next;
                free(cur->previous);
            } else {
                free(cur);
                cur = NULL;
            }
        } else {
            free(current_gs.gs);
            current_gs.gs = cur;
            cur = cur->next;
        }
    } while(cur);
    current_gs.gs->parent = current_gs.gs->next = current_gs.gs->previous = NULL;
    return;
}
void free_branch(game_state *branch)
{
    if(!branch) return;
    free_branch(branch->next);
    free_branch(branch->first);
    free(branch);
    return;
}
void fprint_tree(game_state *gs, int d)
{
    if(!gs) return;
//    int i;
    char *emil = malloc(256);
    //for(i = d; i > 1; i--) DBUG(" |");
    //if(d > 0) DBUG(" L");
    sprint_game_state(emil, gs);
    if(d == 3)DBUG(" %s\n", emil);
    free(emil);
    fprint_tree(gs->first, d+1);
    fprint_tree(gs->next, d);
    return;
}

void start_timer()
{
    if(!stopwatch.running) {
        stopwatch.start = clock();
        stopwatch.running = 1;
    }
}

double get_time()
{
    return (double) (clock() - stopwatch.start) / CLOCKS_PER_SEC;
}

void reset_timer()
{
    stopwatch.running = 0;
}
