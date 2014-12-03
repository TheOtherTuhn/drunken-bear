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


int main(int argc, char** argv) {
	move last_move = malloc(sizeof(*last_move));
	for(int i = 0; i < NTHREADS; i++) {
		pthread_create(&threads[i], NULL, &gen_gs, NULL);
	}
	while(parseline(&last_move)) {
		if(last_move) { /* if first move (last_move == NULL), don't free anything */
			update_current_gs(current_gs.gs, last_move);
			qempty();
			push_leaves(current_gs.gs);
		}
		/* wait 4 alphabeta schmock */
	}
}

int parseline(move *last_move)
{
	char *input, *found;
	int f = 0; /* index of current field */
	while(getline(&input, NULL, stdin) != -1) {
		if(!current_gs.gs) {
			current_gs.gs = malloc(sizeof(*current_gs.gs));
			while(getline(&input, NULL, stdin) != -1 && f < 64) {
				if(found = strstr(input, "WelcomeMessage\" color=\"")) {
					current_gs.we_are_red = (*(found+23)=='r');
				} else if(found = strstr(input, "startPlayer=\"")) {
					current_gs.gs->b_current = (*(found+13)=='B');
					current_gs.gs->r_current = (*(found+13)=='R');
				} else if(found = strstr(input, "field fish=\"")) {
					current_gs.gs->fields[f++] = nfromc(found+12);
				}
			}
			qpush(current_gs.gs);
		}
		if(strstr(input, "lastMove")) {
			if(strstr(input, "NullMove")) {
				last_move.type = move_type.Null;
			} else if(strstr(input, "SetMove")) {
				last_move.type = move_type.Set;
				last_move.to   = nfromc(strstr(input, "setX=\"") + 6) << 3 \
				               | nfromc(strstr(input, "setY=\"") + 6);
			} else if(strstr(input, "RunMove")) {
				last_move.type = move_type.Run;
				last_move.from = nfromc(strstr(input, "fromX=\"") + 7) << 3 \
				               | nfromc(strstr(input, "fromY=\"") + 7);
				last_move.to   = nfromc(strstr(input, "toX=\"") + 5) << 3 \
				               | nfromc(strstr(input, "toY=\"") + 5);
				
			}
		} else if(strstr(input, "MoveRequest")) {
			return 1;
		} else if(strstr(input, "</protocol>")) {
			return 0;
		}
	}
	return 0;
}

int nfromc(char c)
{
	return c - '0';
}
