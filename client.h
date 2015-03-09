#ifndef __CLIENT_H__
#define __CLIENT_H__
/*Defines game logic*/
#define BUFSIZE 256
#define DEBUG 1
#ifdef DEBUG
# if DEBUG == 1
#  define DBUG(s, ...) fprintf(stderr, s, ##__VA_ARGS__)
# elif DEBUG == 2
#  define DBUG(s, ...) fprintf(stderr, "%3d", __LINE__); fprintf(stderr, s, ##__VA_ARGS__)
# else
#  define DBUG(s, ...) fprintf(stderr, "<%s>:%3d: ", __FILE__, __LINE__); fprintf(stderr, s, ##__VA_ARGS)
# endif
#else
# define DBUG(s, ...)
#endif

#include <limits.h>
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "gen_thread.h"
#include "gs.h"
#include "q.h"

struct l_move{
    move n_move; //also used for temporarily strong own move for sending
    move s_move;
    move r_move;
} last_move;

game_state *minmax(game_state*);
void update_current_gs(game_state*, move);
void push_leaves(game_state*);
void* gen_gs(void*);
int parseline(struct l_move*);
uint8_t nfromc(char);
void free_branch(game_state*);
void fprint_tree(game_state*, int);
void start_timer();
double get_time();
void reset_timer();
#endif
