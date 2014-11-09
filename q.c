struct {
	gs_qr* first;
	pthread_mutex_t m;
} gs_q = {NULL, PTHREAD_MUTEX_INITIALIZER};

game_state* qpop(void) {
	pthread_mutex_lock(&gs_q.m);
		if(!gs_q.first)
			return NULL;
		gs_qr* old = gs_q.first;
		game_state* res = old->gs;
		gs_q.first = old->nxt;
		free(old);
	pthread_mutex_unlock(&gs_q.m);
	return res;
}

void qpush(game_state* gs) {
	pthread_mutex_lock(&gs_q.m);
		if(gs_q.first) {
			gs_qr* cur = gs_q.first;
			while(cur->nxt) {
				cur = cur->nxt;
			}
			cur->nxt = malloc(sizeof(gs_qr));
			cur->nxt->gs = gs;
		} else {
			gs_q.first = malloc(sizeof(gs_qr));
			gs_q.first->gs = gs;
		}
	pthread_mutex_unlock(&gs_q.m);
}

void qempty(void) {
	while(qpop());
}

int qlength(void) {
	int n = 1;
	gs_qr* c = gs_q.first;
	if(!c) return 0;
	while((c=c->nxt))n++;
	return n;
}
