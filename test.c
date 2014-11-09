#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "client.h"
#include "proto.h"
#include "gs.c"
#include "q.c"

int main(int argc, char** argv) {
	game_state* new_gs = NULL;
	char* line = "R1R1B111323221111321R221B11231111311211222311233312123221211201020102";
	new_gs = parse_gs(line);
	qpush(new_gs);
	gen_gs(NULL);
	print_gs(new_gs);
	free(new_gs);
	return 0;
}
