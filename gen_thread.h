#ifndef __GEN_THREAD_H__
#define __GEN_THREAD_H__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "gs.h"
#include "q.h"

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

#define E(f)  (f+8)
#define NE(f) (f+(f&1?-1:7))
#define NW(f) (f+(f&1?-9:-1))
#define W(f)  (f-8)
#define SW(f) (f+(f&1?-7:1))
#define SE(f) (f+(f&1?1:9))
#define INBOUNDS(f) ((0<=f)&&(f<64))

typedef enum {Generate, Wait, Exit} ThreadCommand;

void set_thread_command(ThreadCommand);
int get_thread_count(void);
void increase_thread_count(void);
void decrease_thread_count(void);
int generate(game_state*);
void *gen_gs(void*);

#endif
