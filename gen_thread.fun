void increase_thread_count(void)
{
    pthread_mutex_lock(&thread_info.m);
        thread_info.count++;
    pthread_mutex_unlock(&thread_info.m);
}

void decrease_thread_count(void)
{
    pthread_mutex_lock(&thread_info.m);
        thread_info.count--;
    pthread_mutex_unlock(&thread_info.m);
}

/*c function to generate moves*/
int generate(game_state *old)
{
    int i, j;
    int count = 0;
    game_state *prv, *dummy;
    if(old->turn < 8) {
        for(i = 0; i < 64; i++) {
            if(old->fields[i].rpen == old->fields[i].bpen \
            && old->fields[i].fish == 1) {
                if((old->r_current && old->leftR > 0) \
                ||(old->b_current && old->leftB > 0)) {
                    dummy = simulate_set_move(old, prv, i);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
            }
        }
        old->last = prv;
    } else {
        prv = spawn_gs(old);
        prv->last_move = (move) {Null, 0, 0};
        old->first = prv;
        qpush(prv);
        for(i = 0; i < 64; i++) {
            if((old->fields[i].rpen && old->b_current) \
            || (old->fields[i].bpen && old->r_current)) {
                for(j = E(i); INBOUNDS(j) \
                    && old->fields[j].fish \
                    && old->fields[j].bpen == old->fields[j].rpen; j = E(j)) {
                    dummy = simulate_run_move(old, prv, i, j);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
                for(j = NE(i); INBOUNDS(j) \
                    && old->fields[j].fish \
                    && old->fields[j].bpen == old->fields[j].rpen; j = NE(j)) {
                    dummy = simulate_run_move(old, prv, i, j);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
                for(j = NW(i); INBOUNDS(j) \
                    && old->fields[j].fish \
                    && old->fields[j].bpen == old->fields[j].rpen; j = NW(j)) {
                    dummy = simulate_run_move(old, prv, i, j);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
                for(j = W(i); INBOUNDS(j) \
                    && old->fields[j].fish \
                    && old->fields[j].bpen == old->fields[j].rpen; j = W(j)) {
                    dummy = simulate_run_move(old, prv, i, j);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
                for(j = SW(i); INBOUNDS(j) \
                    && old->fields[j].fish \
                    && old->fields[j].bpen == old->fields[j].rpen; j = SW(j)) {
                    dummy = simulate_run_move(old, prv, i, j);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
                for(j = SE(i); INBOUNDS(j) \
                    && old->fields[j].fish \
                    && old->fields[j].bpen == old->fields[j].rpen; j = SE(j)) {
                    dummy = simulate_run_move(old, prv, i, j);
                    qpush(dummy);
                    count++;
                    prv = dummy;
                }
            }
        }
        old->last = prv;
    }
    return count;
}

void *gen_gs(void *arg)
{
    int running = 1;
    game_state *old;
    int *gs_count = malloc(sizeof(int));
    *gs_count = 0;
    increase_thread_count();
    while(1) {
        switch(thread_info.command) {
        case Generate:
            if(!running) {
                increase_thread_count();
                running = 1;
            }
            if(!(old = qpop()) || old->turn > 30)
                usleep(200);
            else
                *gs_count += generate(old);
            break;
        case Exit:
            if(running)
                decrease_thread_count();
            pthread_exit(gs_count);
            break;
        default:
        case Wait:
            if(running) {
                decrease_thread_count();
                running = 0;
            }
            usleep(200);
            break;
        }
    }
    return gs_count;
}
