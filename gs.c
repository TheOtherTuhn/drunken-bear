struct {
	game_state* gs;
	int we_are_red; /* to be read from session.id */
	pthread_mutex_t m;
} current_gs = {NULL,-1,PTHREAD_MUTEX_INITIALIZER};

uint8_t turn = 0;

/* equivalent to a NullMove,
 * Set- or RunMoves will be done to the returned gs
 */
game_state* spawn_gs(game_state* p) {
	game_state* new = malloc(sizeof(game_state));
	memcpy(new->fields, p->fields, 64 * sizeof(field));
	new->parent = p;
	new->turn = p->turn+1;
	/* It's the other players turn unless the last penguin has just been set */
	new->pointsR = p->pointsR ^ ((p->turn != 8)<<7);
	new->pointsB = p->pointsB ^ ((p->turn != 8)<<7);
	return new;
}

/* delete all gamestates unless the fields and the turn# are equivalent */
void free_gs(game_state* gs, field* save, int turn) {
	/* NULLcheck */
	if(!gs)
		return;
	free_gs(gs->next, save, turn);
	if(memcmp(gs->fields, save, 64 * sizeof(field)) || gs->turn!=turn) {
		free_gs(gs->first, save, turn);
		free(gs);
	} else {
		gs->next=NULL;
		gs->previous=NULL;
		set_current_gs(gs);
	}
}

void set_current_gs(game_state* gs) {
	pthread_mutex_lock(&current_gs.m);
		gs->parent = NULL;
		current_gs.gs = gs;
	pthread_mutex_unlock(&current_gs.m);
}

int field_fish(field f) {
	return f & 3;
}

/* 0=B,1=R */
int current_player(game_state* gs) {
	return (gs->pointsR & (1<<7)) >> 7;
}

void set_current_player(game_state* gs, int p) {
	if(p) {
		gs->pointsR |= 1<<7;
		gs->pointsB &= ~(1<<7);
	} else {
		gs->pointsB |= 1<<7;
		gs->pointsR &= ~(1<<7);
	}
}

void run_move(game_state* old, game_state* prv, int from, int to) {
	game_state* new = spawn_gs(old);
	prv->next = new;
	new->previous = prv;
	/* R current player? */
	if(old->pointsR > 0x80)
	/* add points */
		new->pointsR += old->fields[from] & 3;
	else /* B current player */
		new->pointsB += old->fields[from] & 3;
	new->fields[from] = 0; /* move penguin from here ... */
	new->fields[to] |= CPEN(old); /* ...to here */
	new->last_move = (move) {Run, from, to};
	qpush(new);
	prv = new;
}

/* should be the thread, only concept for now */
void* gen_gs(void* arg) {
	game_state* old, *prv;
	int n = 0;
	#ifdef TEST
	//while(1) {
		// /* only two depths */
		//if(!(old = qpop()) || old->turn - turn >= 2) {
			//usleep(200);
			//continue;
		//}
	{
		old = qpop();
	#else
	while(1) {
		if(!(old = qpop()) || old->turn >= 30) {
			usleep(200);
			continue;
		}
	#endif
		/* NullMove */
		DBUG("Generating NullMove...");
		prv = spawn_gs(old);
		prv->last_move = (move) {Null, 0, 0};
		qpush(prv);
		DBUG("Done\n");
		/* SetMove */
		if(current_player(old)?old->leftR:old->leftB) {
		DBUG("Generating SetMoves...\n");
			for(int i=0; i<64;i++) {
				if((old->fields[i] & 12) == NPEN && field_fish(old->fields[i]) == 1) {
					DBUG("SetMove #%d:\n", n++);
					game_state* new = spawn_gs(old);
					prv->next = new;
					new->previous = prv;
					/* remove penguin */
					if(current_player(old))
						new->leftR--;
					else
						new->leftB--;
					/* set penguin */
					new->fields[i] |= CPEN(old);
					new->last_move = (move) {Set, 0, i};
					qpush(new);
					prv = new;
					fprintf(stderr,"%s",sprint_move(new->last_move));
				}
			}
		DBUG("Done\n");
		} else {
		/* RunMove */
			for(int i=0; i < 64; i++) {
				if((old->fields[i] & 12) == CPEN(old)) {
					for(int j=i; INBOUNDS(j) && field_fish(old->fields[j])
								&& (old->fields[j] & 12) == NPEN; j=E(j)) {
						run_move(old, prv, i, j);
					}
					for(int j=i; INBOUNDS(j) && field_fish(old->fields[j])
								&& (old->fields[j] & 12) == NPEN; j=NE(j)){
						run_move(old, prv, i, j);
					}
					for(int j=i; INBOUNDS(j) && field_fish(old->fields[j])
								&& (old->fields[j] & 12) == NPEN; j=NW(j)){
						run_move(old, prv, i, j);
					}
					for(int j=i; INBOUNDS(j) && field_fish(old->fields[j])
								&& (old->fields[j] & 12) == NPEN; j=W(j)) {
						run_move(old, prv, i, j);
					}
					for(int j=i; INBOUNDS(j) && field_fish(old->fields[j])
								&& (old->fields[j] & 12) == NPEN; j=SW(j)){
						run_move(old, prv, i, j);
					}
					for(int j=i; INBOUNDS(j) && field_fish(old->fields[j])
								&& (old->fields[j] & 12) == NPEN; j=SE(j)){
						run_move(old, prv, i, j);
					}
				}
			}
		}
	}
	return NULL;
}

game_state* parse_gs(char* in) {
	int ch, f = -1;
	game_state* new = malloc(sizeof(game_state));
	new->leftB = 4;
	new->leftR = 4;
	for(int i=0; (ch = in[i]); i++) {
		if(ch > 'A') {
			if(f >= 0) {
				if(ch=='R') {
					new->fields[f] |= RPEN;
					new->leftR--;
				} else {
					new->fields[f] |= BPEN;
					new->leftB--;
				}
			} else
				set_current_player(new, (ch=='R'));
		} else
			new->fields[++f] = ch - 0x30;
	}
	new->turn = ++turn;
	return new;
}

void print_gs(game_state* gs) {
	printf(" R:%d/%d%s\tB:%d/%d%s\n\n", gs->pointsR & ~(1<<7), gs->leftR, (gs->pointsR & 0x80)?"*":" ",
										gs->pointsB & ~(1<<7), gs->leftB, (gs->pointsB & 0x80)?"*":" ");
	for(int x = 0; x < 8; x++) {
		if(!(x&1)) printf(" ");
		for(int y = 0; y < 8; y++) {
			switch(gs->fields[x+8*y]) {
				case  0: if(y!=7) printf("0 "); break;
				case  1:
				case  2:
				case  3: printf("%d ", gs->fields[x+8*y]); break;
				case  5:
				case  6:
				case  7: printf("%dB", gs->fields[x+8*y] & 3); break;
				case  9:
				case 10:
				case 11: printf("%dR", gs->fields[x+8*y] & 3); break;
				default: break;
			}
		}
		printf("\n");
	}
	printf("turn #%d\n", gs->turn);
}

char* sprint_move(move m) {
	char* ret = malloc(64);
	switch(m.type) {
		case Null: sprintf(ret, "N\n"); break;
		case Set: sprintf(ret, "S %d %d\n", (m.to & ~7) >> 3, m.to & 7); break;
		case Run: sprintf(ret, "R %d %d %d %d\n", (m.from & ~7) >> 3, m.from & 7, (m.to & ~7) >> 3, m.to & 7); break;
		default: sprintf(ret, "Move not valid! {type:%d, from:%d, to:%d}\n", m.type, m.from, m.to); break;
	}
	return ret;
}
