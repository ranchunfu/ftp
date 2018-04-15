SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,${SOURCES})
client:${OBJS}
	gcc $^ -o $@ -lcrypt -lcrypto

clean:
	rm client ${OBJS}
