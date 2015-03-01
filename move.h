#ifndef __MOVE_H__
#define __MOVE_H__
/*Structure for saving moves*/
#include <stdint.h>
#include <stdio.h>
typedef enum { Null = 1, Set, Run} move_type;

typedef struct {
    move_type type;
    uint8_t from, to;
} move;

int get_to_x_move(move);
int get_to_y_move(move);
int get_from_x_move(move);
int get_from_y_move(move);

/* about 7B */
int sprint_move(char*, move);
/* not more than 200B */
int sprint_move_xml(char*, move, char*);
#endif
