CC=gcc
CFLAGS= -Wall -g
OTHERS= amqpFunc.c

################################

OBJS=amqpFunc.o
MAIN=main.c

################################

.PHONY: clean

all: ep

amqpFunc.o: amqpFunc.h
ep3.o: main.c

ep: ${OBJS} 
		${CC} ${CFLAGS} -o server ${MAIN} ${OTHERS}
		@rm -rf *.o *.dSYM .vscode

clean:
	@rm -rf server