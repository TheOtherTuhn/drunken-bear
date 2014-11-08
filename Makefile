CFLAGS?=-Wall -Os -std=c99 -D _GNU_SOURCE
LDADD?=-pthread

normal:
	$(CC) -o client client.c $(CFLAGS) $(LDADD) $(LDFLAGS)

debug:
	$(CC) -o client-debug client.c $(CFLAGS) -g -DDEBUG $(LDADD)

proto: $(wildcard *.c)
	cat *.c | egrep '^(void|int|char|unsigned|game_state)' | sed -r 's/\) \{/);/' > proto.h

all: proto normal debug

clean:
	rm -f client client-debug
