#include "gs.h"

game_state *spawn_gs(game_state *p)
{
    game_state *new = malloc(sizeof(game_state));
    memcpy(new->fields, p->fields, 64 * sizeof(field));
    new->parent = p;
    new->turn = p->turn+1;
    /* It's the other player's turn unless the last penguin has just been set */
    if(new->turn != 8) {
        new->r_current = !p->r_current;
        new->b_current = !p->b_current;
    }
    return new;
}

int get_current_player(game_state *gs)
{
    return gs->r_current;
}

void set_current_player(game_state *gs, int p)
{
    gs->r_current = p;
    gs->b_current =!p;
    return;
}

int rate_gs(game_state *gs)
{
    return (long) gs; /* return pointer in hopes of a semirandom number */
}

game_state *simulate_set_move(game_state *par, game_state *prv, int f)
{
    game_state *new = spawn_gs(par);
    if(!par->first)
        par->first = new;
    else
        prv->next = new;
    new->previous = prv;
    if(get_current_player(new) == PLAYER_RED) {
        new->leftR--;
        new->fields[f].rpen = 1;
    } else {
        new->leftB--;
        new->fields[f].bpen = 1;
    }
    new->last_move = (move) {Set, 0, f};
    return new;
}

game_state *simulate_run_move(game_state *par, game_state *prv, int from, int to)
{
    game_state *new = spawn_gs(par);
    prv->next = new;
    new->previous = prv;
    if(get_current_player(new) == PLAYER_RED) {
        new->pointsR += par->fields[from].fish;
        new->fields[to].rpen = 1;
    } else {
        new->pointsB += new->fields[from].fish;
        new->fields[to].bpen = 1;
    }
    new->fields[from] = (field) {0, 0, 0};
    new->last_move = (move) {Run, from, to};
    return new;
}

int sprint_game_state(char *str, game_state *gs)
{
    unsigned x, y, n;
    n = sprintf(str, " R:%d/%d%c\tB:%d/%d%c\n", gs->pointsR, gs->leftR, gs->r_current?'*':' ',\
            gs->pointsB, gs->leftB, gs->b_current?'*':' ');
    n += sprintf(str+n, "turn #%d\n", gs->turn);
    for(x = 0; x < 8; x++) {
        if(!(x&1)) n+=sprintf(str+n, " ");
        for(y = 0; y < 8; y++) {
            n += sprintf(str+n, "%d%c", gs->fields[x+8*y].fish,\
                    gs->fields[x+8*y].rpen?'R':(gs->fields[x+8*y].bpen?'B':' '));
        }
        n += sprintf(str+n, "\n");
    }
    n += sprint_move(str+n, gs->last_move);
    return n;
}
