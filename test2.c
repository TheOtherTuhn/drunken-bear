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

pthread_t threads[3];

int main(int argc, char** argv) {
	game_state* new_gs = NULL;
	char* line = "R1R1B111323221111321R221B1123111131R121B1222311R233312123221211B201020102";
	turn = 9;
	for(int i = 0; i < 3; i++) {
		pthread_create(&threads[i], NULL, &gen_gs, NULL);
	}
	new_gs = parse_gs(line);
	new_gs->last_move = (move){Set, 0, 10};
	qpush(new_gs);
	for(int i = 0; i < 3; i++) {
		pthread_join(threads[i], NULL);
	}
	print_moves(new_gs, 0);
	free(new_gs);
	return 0;
}
