#ifndef __GS_H__
#define __GS_H__
/*Defines types to store game data*/
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "move.h"
#define PLAYER_RED 1
#define PLAYER_BLUE 0

typedef struct {
    uint8_t rpen:1, bpen:1, fish:2;
} field;

typedef struct _gs {
    field fields[64];
    int rating;
    uint8_t r_current:1, b_current:1; /*red/blue is current player*/
    uint8_t pointsR:7, pointsB:7;
    uint8_t turn; /*never greater than 30*/
    uint8_t leftR:4; /*penguins left to be set*/
    uint8_t leftB:4;
    move last_move; /*move that produced this gamestate*/
    /*pointers for tree structure*/
    struct _gs *parent;
    struct _gs *next, *previous; /*siblings*/
    struct _gs *first, *last; /*children*/
} game_state;

/*Simulates a NullMove
 * -> Advances the game*/
game_state *spawn_gs(game_state*);
/*Called upon a move request
 * Deletes gamestates that are now unnecessary */
int get_current_player(game_state*);
void set_current_player(game_state*, int);
int rate_gs(game_state*);
game_state *simulate_set_move(game_state*, game_state*, int); 
game_state *simulate_run_move(game_state*, game_state*, int, int);
/* about 170bytes */
int sprint_game_state(char*, game_state*);
#endif
