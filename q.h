#ifndef __Q_H__
#define __Q_H__
/*Data structure for move generation threads*/
#include <pthread.h>
#include "gs.h"

game_state *qpop(void);
void qpush(game_state*);
void qempty(void);
int qlength(void);
#endif
