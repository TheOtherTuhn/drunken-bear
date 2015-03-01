#ifndef __CLIENT_H__
#define __CLIENT_H__
/*Defines game logic*/
#define BUFSIZE 256
#define E(f)  (f+8)
#define NE(f) (f+(f&1)?-1:7)
#define NW(f) (f+(f&1)?-9:-1)
#define W(f)  (f-8)
#define SW(f) (f+(f&1)?-7:1)
#define SE(f) (f+(f&1)?1:9)
#define INBOUNDS(f) ((0<=f)&&(f<64))
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
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gs.h"
#include "q.h"

game_state *minmax(game_state*);
void update_current_gs(game_state*, move);
void* gen_gs(void*);
int parseline(move*);
uint8_t nfromc(char);
void fprint_tree(game_state*, int);
#endif
