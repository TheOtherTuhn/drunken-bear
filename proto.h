/* client.c */
int main(int argc, char** argv);
int parseline(move *last_move);
uint8_t nfromc(char c);
/* gs.c */
game_state* spawn_gs(game_state* p);
void update_current_gs(game_state* gs, move save);
void free_branch(game_state *gs);
void push_leaves(game_state *gs);
int field_fish(field f);
int current_player(game_state* gs);
void set_current_player(game_state* gs, int p);
void* gen_gs(void* arg);
void print_gs(game_state* gs);
char* sprint_move(move m);
void print_moves(game_state* gs, int d);
/* q.c */
game_state* qpop(void);
void qpush(game_state* gs);
void qempty(void);
int qlength(void);
