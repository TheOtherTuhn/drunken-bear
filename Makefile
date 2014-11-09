CFLAGS?=-Wall -Os -std=c99 -D_GNU_SOURCE
LDADD?=-pthread

normal:
	$(CC) -o client client.c $(CFLAGS) $(LDADD) $(LDFLAGS)

debug:
	$(CC) -o client-debug client.c $(CFLAGS) -g -DDEBUG $(LDADD)

test:
	$(CC) -o client-test test.c $(CFLAGS) -DTEST $(LDADD) $(LDFLAGS)

proto: gs.c q.c client.c
	cat *.c | egrep '^(void|int|char|unsigned|game_state)' | sed -r 's/\) \{/);/' > proto.h

all: proto normal debug

clean:
	rm -f client client-debug
