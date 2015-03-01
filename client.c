#include "client.h"

struct {
    game_state *gs;
    int we_are_red;
    char sid[37];
    int turn;
} current_gs = {NULL, -1, {0}, 0};

int main(int argc, char *argv[])
{
    move *last_move = malloc(sizeof(move));
    last_move->type = Null;
    last_move->from = last_move-> to = 0;
    char *emil = malloc(256);
    parseline(last_move);
    sprint_game_state(emil, current_gs.gs);
    printf("%s\n", emil);
    sprint_move(emil, *last_move);
    printf("%s\n", emil);
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, &gen_gs, NULL);
    sleep(2);
    pthread_cancel(*thread);
//    fprint_tree(current_gs.gs, 0);
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
    while(getdelim(&input, &size,'>', stdin) != -1 && f < 64) {
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

void fprint_tree(game_state *gs, int d)
{
    if(!gs) return;
    int i;
    char *emil = malloc(8);
    for(i = d; i > 1; i--) DBUG(" |");
    if(d > 0) DBUG(" L");
    sprint_move(emil, gs->last_move);
    DBUG(" %s\n", emil);
    free(emil);
    fprint_tree(gs->first, d+1);
    fprint_tree(gs->next, d);
    return;
}
