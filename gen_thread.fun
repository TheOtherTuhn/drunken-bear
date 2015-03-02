/*c function to generate moves*/
void *gen_gs(void *arg)
{
    int i, j;
    game_state *prv, *old, *dummy;
    while(1) {
        if(!(old = qpop()) || old->turn > 30) {
            usleep(200);
            continue;
        }
        if(old->turn < 8) {
            for(i = 0; i < 64; i++) {
                if(old->fields[i].rpen == old->fields[i].bpen \
                && old->fields[i].fish == 1) {
                    if((old->r_current && old->leftR > 0) \
                    ||(old->b_current && old->leftB > 0)) {
                        dummy = simulate_set_move(old, prv, i);
                        n_gen++;
                        qpush(dummy);
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
                        simulate_run_move(old, prv, i, j);
                    }
                    for(j = NE(i); INBOUNDS(j) \
                        && old->fields[j].fish \
                        && old->fields[j].bpen == old->fields[j].rpen; j = NE(j)) {
                        simulate_run_move(old, prv, i, j);
                    }
                    for(j = NW(i); INBOUNDS(j) \
                        && old->fields[j].fish \
                        && old->fields[j].bpen == old->fields[j].rpen; j = NW(j)) {
                        simulate_run_move(old, prv, i, j);
                    }
                    for(j = W(i); INBOUNDS(j) \
                        && old->fields[j].fish \
                        && old->fields[j].bpen == old->fields[j].rpen; j = W(j)) {
                        simulate_run_move(old, prv, i, j);
                    }
                    for(j = SW(i); INBOUNDS(j) \
                        && old->fields[j].fish \
                        && old->fields[j].bpen == old->fields[j].rpen; j = SW(j)) {
                        simulate_run_move(old, prv, i, j);
                    }
                    for(j = SE(i); INBOUNDS(j) \
                        && old->fields[j].fish \
                        && old->fields[j].bpen == old->fields[j].rpen; j = SE(j)) {
                        simulate_run_move(old, prv, i, j);
                    }
                }
            }
            old->last = prv;
        }
    }
    return NULL;
}
