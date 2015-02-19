/*Defines game logic*/

struct {
    game_state *gs;
    int we_are_red;
    char sid[36];
    int turn;
} current_gs = {NULL, -1, {0}, 0};

game_state *minmax(game_state*);
void update_current_gs(game_state*, move);
int parseline(move*);
uint8_t nfromc(char);
