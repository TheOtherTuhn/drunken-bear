struct {
	game_state* gs;
	int we_are_red; /* to be read from session.id */
	pthread_mutex_t m;
} curgs = {NULL,-1,PTHREAD_MUTEX_INITIALIZER};

game_state* spawn_gs(game_state* par) {
	game_state* new = malloc(sizeof(game_state));
	memcpy(new->fds, par->fds, 64 * sizeof(field));
	new->par = par;
	new->trn = par->trn+1;
	new->ptsR ^= 1<<7;
	new->ptsB ^= 1<<7;
	return new;
}

void free_gs(game_state* gs, field* save) {
	if(gs==NULL)
		return;
	free_gs(gs->nxt, save);
	if(memcmp(gs->fds, save, 64 * sizeof(field))) {
		free_gs(gs->fst, save);
		free(gs);
	} else {
		gs->nxt=NULL;
		gs->prv=NULL;
		cur_gs(gs);
	}
}

void cur_gs(game_state* gs) {
	pthread_mutex_lock(&curgs.m);
		gs->par = NULL;
		curgs.gs = gs;
	pthread_mutex_unlock(&curgs.m);
}

int field_fish(field f) {
	if(f < 0 || f > 64)
		return 0;
	return f & 3;
}

/* 0=B,1=R */
int curp(game_state* gs) {
	return (gs->ptsR & (1<<7)) >> 7;
}

void set_curp(game_state* gs, int p) {
	if(p) {
		gs->ptsR |= 1<<7;
		gs->ptsB &= ~(1<<7);
	} else {
		gs->ptsB |= 1<<7;
		gs->ptsR &= ~(1<<7);
	}
}

/* p: 0=B,1=R */
int pens_left(game_state* gs, int p) {
	return (gs->lft & (15<<(p*4)))>>(p*4);
}

/* should be the thread, only concept for now */
void* gen_gs(void* arg) {
	game_state* old, *prv;
	while(1) {
		if(!(old = qpop())|| (old->trn & 0x0f) >= 30)
			continue;
		/* NullMove */
		prv = spawn_gs(old);
		prv->lmove = (move) {Null, 0, 0};
		qpush(prv);
		/* SetMove */
		if(pens_left(old,curp(old))) {
			for(int i=0; i<64;i++) {
				if((old->fds[i] & 12) == NPEN && field_fish(old->fds[i]) == 1) {
					game_state* new = spawn_gs(old);
					prv->nxt = new;
					new->prv = prv;
					new->fds[i] |= CPEN(old);
					new->lft = ((pens_left(old, 1)<<4) - curp(old)) | (pens_left(old, 0) - !curp(old));
					new->lmove = (move) {Set, 0, i};
					qpush(new);
					prv = new;
				}
			}
		} else {
		/* RunMove */
			for(int i=0; i < 64; i++) {
				if((old->fds[i] & 12) == CPEN(old)) {
					for(int j=i; field_fish(old->fds[j]) && (old->fds[j] & 12) == NPEN; j=E(j)) {
						game_state* new = spawn_gs(old);
						prv->nxt = new;
						new->prv = prv;
						if(old->ptsR > 0x80)
							new->ptsR += old->fds[i];
						else
							new->ptsB += old->fds[i];
						new->fds[i] = 0;
						new->fds[j] |= CPEN(old);
						new->lmove = (move) {Run, i, j};
						qpush(new);
						prv = new;
					}
					for(int j=i; field_fish(old->fds[j]) && (old->fds[j] & 12) == NPEN; j=NE(j)) {
						game_state* new = spawn_gs(old);
						prv->nxt = new;
						new->prv = prv;
						if(old->ptsR > 0x80)
							new->ptsR += old->fds[i];
						else
							new->ptsB += old->fds[i];
						new->fds[i] = 0;
						new->fds[j] |= CPEN(old);
						new->lmove = (move) {Run, i, j};
						qpush(new);
						prv = new;
					}
					for(int j=i; field_fish(old->fds[j]) && (old->fds[j] & 12) == NPEN; j=NW(j)) {
						game_state* new = spawn_gs(old);
						prv->nxt = new;
						new->prv = prv;
						if(old->ptsR > 0x80)
							new->ptsR += old->fds[i];
						else
							new->ptsB += old->fds[i];
						new->fds[i] = 0;
						new->fds[j] |= CPEN(old);
						new->lmove = (move) {Run, i, j};
						qpush(new);
						prv = new;
					}
					for(int j=i; field_fish(old->fds[j]) && (old->fds[j] & 12) == NPEN; j=W(j)) {
						game_state* new = spawn_gs(old);
						prv->nxt = new;
						new->prv = prv;
						if(old->ptsR > 0x80)
							new->ptsR += old->fds[i];
						else
							new->ptsB += old->fds[i];
						new->fds[i] = 0;
						new->fds[j] |= CPEN(old);
						new->lmove = (move) {Run, i, j};
						qpush(new);
						prv = new;
					}
					for(int j=i; field_fish(old->fds[j]) && (old->fds[j] & 12) == NPEN; j=SW(j)) {
						game_state* new = spawn_gs(old);
						prv->nxt = new;
						new->prv = prv;
						if(old->ptsR > 0x80)
							new->ptsR += old->fds[i];
						else
							new->ptsB += old->fds[i];
						new->fds[i] = 0;
						new->fds[j] |= CPEN(old);
						new->lmove = (move) {Run, i, j};
						qpush(new);
						prv = new;
					}
					for(int j=i; field_fish(old->fds[j]) && (old->fds[j] & 12) == NPEN; j=SE(j)) {
						game_state* new = spawn_gs(old);
						prv->nxt = new;
						new->prv = prv;
						if(old->ptsR > 0x80)
							new->ptsR += old->fds[i];
						else
							new->ptsB += old->fds[i];
						new->fds[i] = 0;
						new->fds[j] |= CPEN(old);
						new->lmove = (move) {Run, i, j};
						qpush(new);
						prv = new;
					}
				}
			}
		}
	}
}

game_state* parse_gs(char* in) {
	int rleft, bleft, ch, f = -1;
	rleft = bleft = 4;
	game_state* new = malloc(sizeof(game_state));
	for(int i=0; (ch = in[i]); i++) {
		if(ch > 'A') {
			if(f >= 0) {
				if(ch=='R') {
					new->fds[f] |= RPEN;
					rleft--;
				} else {
					new->fds[f] |= BPEN;
					bleft--;
				}
			} else
				set_curp(new, (ch=='R'));
		} else
			new->fds[++f] = ch - 0x30;
	}
	new->trn = ++turn;
	new->lft = (rleft << 4) | bleft;
	return new;
}

void print_gs(game_state* gs) {
	int fld = 0;
	field* fds = gs->fds;
	printf(" ");
	while(fld < 64) {
		if(fds[fld] & RPEN)
			printf("r ");
		else if(fds[fld] & BPEN)
			printf("b ");
		else
			printf("%d ", fds[fld]);
		if ((++fld & 7) == 0) {
			printf("\n");
			if ((fld & 15) == 0)
				printf(" ");
		}
	}
}

void print_move(move m) {
	switch(m.t) {
		case Null: printf("N\n"); break;
		case Set: printf("S %d %d\n", (m.to & ~7) >> 3, m.to & 7); break;
		case Run: printf("R %d %d %d %d\n", (m.from & ~7) >> 3, m.from & 7, (m.to & ~7) >> 3, m.to & 7); break;
		default: printf("Move not valid! {t:%d, from:%d, to:%d}\n", m.t, m.from, m.to); break;
	}
}
