/* red, blue, no, current penguin on field */
#define RPEN 0x08
#define BPEN 0x04
#define NPEN 0x00
#define CPEN(old) 1<<(2+current_player(old))
/* east, northeast, northwest
 * west, southwest, southeast
 * of f
 */
#define E(f)  (f+8)
#define NE(f) (f+(f&1)?-1:7)
#define NW(f) (f+(f&1)?-9:-1)
#define W(f)  (f-8)
#define SW(f) (f+(f&1)?-7:1)
#define SE(f) (f+(f&1)?1:9)
#define INBOUNDS(f) ((0<=f)&&(f<64))

#define NTHREADS 8

#ifdef TEST
#define DBUG(s, ...) fprintf(stderr, s, ##__VA_ARGS__)
#else
#define DBUG(s, ...)
#endif

/* 0b0000ppnn
 * pp - any one of RPEN, BPEN, NPEN, meaning red, blue or no penguin
 * nn - # of fish, zero if no field */
typedef struct {
	uint8_t rpen:1,bpen:1,fish:2;
} field;

typedef enum { Null, Set, Run } move_type;

typedef struct {
	move_type type;
	uint8_t from, to;
} move;

typedef struct _gs {
	field fields[64];
	uint8_t r_current:1, b_current:1; /* red/blue is current player */
	uint8_t pointsR:7, pointsB:7;
	uint8_t turn; /* never greater than 30 */
	uint8_t leftR:4; /* penguins left to be set (R=high nibble;B=low nibble) */
	uint8_t leftB:4;
	move last_move; /* the move that produced this gs */
	struct _gs* parent; /* parent */
	struct _gs* next; /* next sibling */
	struct _gs* previous; /* previous sibling */
	struct _gs* first; /* first child */
	struct _gs* last; /* last child */
} game_state;

/* standing in the q */
typedef struct _qr {
	game_state* gs;
	struct _qr* next;
} gs_qr;
