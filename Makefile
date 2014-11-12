CFLAGS?=-Wall -Werror -Os -std=c99 -D_GNU_SOURCE
LDADD?=-pthread

normal:
	$(CC) -o client client.c $(CFLAGS) $(LDADD) $(LDFLAGS)

debug:
	$(CC) -o client-debug client.c $(CFLAGS) -g -DDEBUG $(LDADD)

test1:
	$(CC) -o client-test test1.c $(CFLAGS) -DTEST $(LDADD) $(LDFLAGS)
test2:
	$(CC) -o client-test test2.c $(CFLAGS) -DTEST $(LDADD) $(LDFLAGS)

proto: gs.c q.c client.c
	cat *.c | egrep '^(void|int|char|unsigned|game_state)' | sed -r 's/\) \{/);/' > proto.h

all: proto normal debug

clean:
	rm -f client client-debug
