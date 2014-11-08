/* red, blue, no, current penguin on field */
#define RPEN 0x08
#define BPEN 0x04
#define NPEN 0x00
#define CPEN(old) 1<<(2+curp(old))
/* east, northeast, northwest
 * west, southwest, southeast
 * of f
 */
#define E(f) f+8
#define NE(f) f+(f&1)?-1:7
#define NW(f) f+(f&1)?-9:-1
#define W(f) f-8
#define SW(f) f+(f&1)?-7:1
#define SE(f) f+(f&1)?1:9

#define NTHREADS 8

/* 0b0000ppnn
 * pp - any one of RPEN, BPEN, NPEN, meaning red, blue or no penguin
 * nn - # of fish, zero if no field */
typedef uint8_t field;

typedef enum { Null, Set, Run } move_type;

typedef struct {
	move_type t;
	uint8_t from, to;
} move;

typedef struct _gs {
	field fds[64];
	uint8_t ptsR, ptsB; /* never greater than 100, highest bit set if
							current player */
	uint8_t trn; /* never greater than 30
					=> highest nibble contains penguins left to be set*/
	move lmove; /* the move that produced this gs */
	struct _gs* par; /* parent */
	struct _gs* nxt; /* next sibling */
	struct _gs* prv; /* previous sibling */
	struct _gs* fst; /* first child */
	struct _gs* lst; /* last child */
} game_state;

typedef struct _qr {
	game_state* gs;
	struct _qr* nxt;
} gs_qr;

uint8_t turn = 0;
