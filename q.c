#include "q.h"

typedef struct _qr {
    game_state *gs;
    struct _qr *next;
} gs_qr;

struct {
    gs_qr *first, *last;
    pthread_mutex_t m;
} gs_q = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

void qpush(game_state *gs)
{
    pthread_mutex_lock(&gs_q.m);
        if(gs_q.first) {
            gs_q.last->next = malloc(sizeof(gs_qr));
            gs_q.last->next->gs = gs;
            gs_q.last = gs_q.last->next;
        } else {
            gs_q.first = malloc(sizeof(gs_qr));
            gs_q.first->gs = gs;
            gs_q.last = gs_q.first;
        }
    pthread_mutex_unlock(&gs_q.m);
}

game_state *qpop(void)
{
    if(!gs_q.first)
        return NULL;
    pthread_mutex_lock(&gs_q.m);
        gs_qr *old = gs_q.first;
        game_state *ret = old->gs;
        gs_q.first = old->next;
        free(old);
    pthread_mutex_unlock(&gs_q.m);
    return ret;
}

void qempty(void)
{
    gs_qr *current = gs_q.first;
    gs_qr *dummy;
    pthread_mutex_lock(&gs_q.m);
        while(current) {
           dummy = current->next;
           free(current);
           current = dummy;
        }
        gs_q.first = gs_q.last = NULL;
    pthread_mutex_unlock(&gs_q.m);
}

int qlength(void)
{
    int n = 1;
    gs_qr *c = gs_q.first;
    if(!c) return 0;
    while((c=c->next)) n++;
    return n;
}
