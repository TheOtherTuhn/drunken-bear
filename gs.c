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
	if(new->turn != 8) {
		new->r_current = !p->r_current;
		new->b_current = !p->b_current;
	}
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

/* 0=B,1=R */
int current_player(game_state* gs) {
	return gs->r_current;
}

void set_current_player(game_state* gs, int p) {
	gs->r_current = p;
	gs->b_current =!p;
}

void run_move(game_state* old, game_state** prv, int from, int to) {
	game_state* new = spawn_gs(old);
	(*prv)->next = new;
	new->previous = *prv;
	if(old->r_current)
		new->pointsR += old->fields[from].fish;
	else
		new->pointsB += old->fields[from].fish;
	new->fields[from].fish = new->fields[from].rpen = new->fields[from].bpen = 0;
	new->fields[to].rpen = old->r_current;
	new->fields[to].bpen = old->b_current;
	new->last_move = (move) {Run, from, to};
	qpush(new);
	*prv = new;
}

void* gen_gs(void* arg) {
	game_state* old, *prv = NULL;
	int n = 0;
	#ifdef TEST
	while(1) {
		/* only two depths */
		if(!(old = qpop())) {
			/*usleep(200);*/
			break;
		}
		if(old->turn - turn >= 3)
			continue;
	/*{
		old = qpop();*/
	#else
	while(1) {
		if(!(old = qpop()) || old->turn >= 30) {
			usleep(200);
			continue;
		}
	#endif
		/* SetMove */
		if(old->turn <= 8) {
			DBUG("Generating SetMoves...\n");
			for(int i=0; i<64;i++) {
				if(old->fields[i].rpen == old->fields[i].bpen \
				&& old->fields[i].fish == 1) {
					DBUG("SetMove #%d:\n", n++);
					game_state* new = spawn_gs(old);
					if(n++==0) old->first = new;
					prv->next = new;
					new->previous = prv;
					/* remove penguin */
					if(old->r_current)
						new->leftR--;
					else
						new->leftB--;
					/* set penguin */
					new->fields[i].rpen = old->r_current;
					new->fields[i].bpen = old->b_current;
					new->last_move = (move) {Set, 0, i};
					qpush(new);
					prv = new;
					DBUG("%s",sprint_move(new->last_move));
				}
			}
			DBUG("Done\n");
		} else {
		/* NullMove */
			DBUG("Generating NullMove...");
			prv = spawn_gs(old);
			prv->last_move = (move) {Null, 0, 0};
			old->first = prv;
			qpush(prv);
			DBUG("Done\n");
		/* RunMove */
			DBUG("Generating SetMoves...\n");
			for(int i=0; i < 64; i++) {
				if((old->fields[i].rpen && old->r_current) \
				|| (old->fields[i].bpen && old->b_current)) {
					for(int j=E(i); INBOUNDS(j) \
						&& old->fields[j].fish \
						&& old->fields[j].rpen == old->fields[j].bpen; j=E(j)) {
						run_move(old, &prv, i, j);
					}
					for(int j=NE(i); INBOUNDS(j) \
						&& old->fields[j].fish \
						&& old->fields[j].rpen == old->fields[j].bpen; j=NE(j)){
						run_move(old, &prv, i, j);
					}
					for(int j=NW(i); INBOUNDS(j) \
						&& old->fields[j].fish \
						&& old->fields[j].rpen == old->fields[j].bpen; j=NW(j)){
						run_move(old, &prv, i, j);
					}
					for(int j=W(i); INBOUNDS(j) \
						&& old->fields[j].fish \
						&& old->fields[j].rpen == old->fields[j].bpen; j=W(j)) {
						run_move(old, &prv, i, j);
					}
					for(int j=SW(i); INBOUNDS(j) \
						&& old->fields[j].fish \
						&& old->fields[j].rpen == old->fields[j].bpen; j=SW(j)){
						run_move(old, &prv, i, j);
					}
					for(int j=SE(i); INBOUNDS(j) \
						&& old->fields[j].fish \
						&& old->fields[j].rpen == old->fields[j].bpen; j=SE(j)){
						run_move(old, &prv, i, j);
					}
				}
			}
			DBUG("Done\n");
		}
		prv->parent->last = prv;
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
					new->fields[f].rpen = 1;
					new->leftR--;
				} else {
					new->fields[f].bpen = 1;
					new->leftB--;
				}
			} else
				set_current_player(new, (ch=='R'));
		} else
			new->fields[++f].fish = ch - '0';
	}
	new->turn = ++turn;
	return new;
}

void print_gs(game_state* gs) {
	printf(" R:%d/%d%c\tB:%d/%d%c\n", gs->pointsR, gs->leftR, gs->r_current?'*':' '\
					  , gs->pointsB, gs->leftB, gs->b_current?'*':' ');
	printf("turn #%d\n", gs->turn);
	for(int x = 0; x < 8; x++) {
		if(!(x&1)) printf(" ");
		for(int y = 0; y < 8; y++) {
			printf("%d%c", gs->fields[x+8*y].fish,
				gs->fields[x+8*y].rpen ? 'R' : (gs->fields[x+8*y].bpen ? 'B' : ' '));
		}
		printf("\n");
	}
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

void print_moves(game_state* gs, int d) {
	if(!gs) return;
	for(int i=d;i>1;i--) DBUG(" |");
	if(d>0)DBUG(" L ");
	DBUG("%s", sprint_move(gs->last_move));
	print_moves(gs->first, d+1);
	print_moves(gs->next, d);
}
