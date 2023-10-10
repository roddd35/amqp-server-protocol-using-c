CC=gcc
CFLAGS= -Wall -g
OTHERS= amqpFunc.c connection.c

################################

OBJS=amqpFunc.o connection.o
MAIN=main.c

################################

.PHONY: clean

all: ep

amqpFunc.o: amqpFunc.h
connection.o: connection.h
ep3.o: main.c

ep: ${OBJS} 
		${CC} ${CFLAGS} -o server ${MAIN} ${OTHERS}
		@rm -rf *.o *.dSYM .vscode .DS_Store

clean:
	@rm -rf server