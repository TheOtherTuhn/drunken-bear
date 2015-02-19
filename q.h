/*Data structure for move generation threads*/

typedef struct _qr {
    game_state *gs;
    struct _qr *next;
} gs_qr;

struct {
    gs_qr *first, *last;
    pthread_mutex_t m;
} gs_q = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

game_state *qpop(void);
void qpush(game_state*);
void qempty(void);
int qlength(void);
