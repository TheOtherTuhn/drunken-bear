#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "proto.h"
#include "gs.c"
#include "q.c"

pthread_t threads[NTHREADS];

#define BUFSIZE 100


int main(int argc, char** argv) {
	move *last_move = calloc(1, sizeof(move));
	for(int i = 0; i < NTHREADS; i++) {
		pthread_create(&threads[i], NULL, &gen_gs, NULL);
	}
	while(parseline(last_move)) {
        printf("%s", sprint_move(*last_move));
		if(last_move->type!=0) {
            DBUG("\n");
			update_current_gs(current_gs.gs, *last_move);
            print_gs(current_gs.gs);
			qempty();
			push_leaves(current_gs.gs);
		}
		/* wait 4 alphabeta schmock */
	}
}

int parseline(move *last_move)
{
    size_t size = BUFSIZE * sizeof(char);
	char *input = malloc(size);
    char *found;
    int r = 0;
	int f = 0; /* index of current field */
	while(getdelim(&input, &size, '>', stdin) != -1) {
        DBUG("%s\n", input);
		if(!current_gs.gs) {
			current_gs.gs = malloc(sizeof(*current_gs.gs));
			while(getdelim(&input, &size, '>', stdin) != -1 && f < 64) {
				if((found = strstr(input, "WelcomeMessage\" color=\""))) {
					current_gs.we_are_red = (found[23]=='r');
				} else if((found = strstr(input, "startPlayer=\""))) {
					current_gs.gs->b_current = (found[13]=='B');
					current_gs.gs->r_current = (found[13]=='R');

				} else if((found = strstr(input, "field fish=\""))) {
					current_gs.gs->fields[f++].fish = nfromc(*found+12);
                }
			}
            DBUG("\n");
			qpush(current_gs.gs);
            DBUG("\n");
		}
		if(strstr(input, "lastMove")) {
            DBUG("\n");
			if(strstr(input, "NullMove")) {
				last_move->type = Null;
			} else if(strstr(input, "SetMove")) {
				last_move->type = Set;
				last_move->to   = nfromc(strstr(input, "setX=\"")[6]) << 3 \
				                | nfromc(strstr(input, "setY=\"")[6]);
                DBUG("%d %d\n", move_to_x(*last_move), move_to_y(*last_move));
			} else if(strstr(input, "RunMove")) {
				last_move->type = Run;
				last_move->from = nfromc(*strstr(input, "fromX=\"") + 7) << 3 \
				                | nfromc(*strstr(input, "fromY=\"") + 7);
				last_move->to   = nfromc(*strstr(input, "toX=\"") + 5) << 3 \
				                | nfromc(*strstr(input, "toY=\"") + 5);
				
			}
		} else if(strstr(input, "MoveRequest")) {
            r = 1;
            goto end;
		} else if(strstr(input, "</protocol>")) {
            goto end;
		}
	}
end:
    DBUG("\n");
    free(input);
    input = NULL;
    return r;
}

uint8_t nfromc(char c)
{
	return c - '0';
}
