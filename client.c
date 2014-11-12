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

pthread_t gen_thd[NTHREADS];

int main(int argc, char** argv) {
	game_state* new_gs = NULL;
	char* line;
	size_t size;
	for(int i = 0; i < NTHREADS; i++) {
		gen_thd[i] = pthread_create(gen_thd+i, NULL, &gen_gs, NULL);
	}
	while(getline(&line, &size, stdin) != -1) {
		new_gs = parse_gs(line);
		qempty();
		free_gs(current_gs.gs, new_gs->fields);
		qpush(new_gs);
	}
	free(new_gs);
	return 0;
}
