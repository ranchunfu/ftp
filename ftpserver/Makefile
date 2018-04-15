SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,${SOURCES})
ftpserver:${OBJS}
	gcc $^ -o $@ -I /usr/include/mysql/ -lmysqlclient -lcrypt

clean:
	rm ftpserver ${OBJS}
