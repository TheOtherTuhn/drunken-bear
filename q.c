struct {
	gs_qr* first;
	pthread_mutex_t m;
} gs_q = {NULL, PTHREAD_MUTEX_INITIALIZER};

game_state* qpop(void) {
	if(!gs_q.first) /* q is empty */
		return NULL;
	pthread_mutex_lock(&gs_q.m);
		gs_qr* old = gs_q.first;
		game_state* res = old->gs; /* we're only interested in the gs */
		gs_q.first = old->next; /* advance q */
		free(old); /* we're only interested in the gs */
	pthread_mutex_unlock(&gs_q.m);
	return res;
}

void qpush(game_state* gs) {
	pthread_mutex_lock(&gs_q.m);
		if(gs_q.first) {
			gs_qr* cur = gs_q.first;
			while(cur->next) { /* advance to the last qr */
				cur = cur->next;
			}
			cur->next = malloc(sizeof(gs_qr));
			cur->next->gs = gs;
		} else { /* q is empty */
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
	while((c=c->next))n++;
	return n;
}
