CC=gcc
CFLAGS= -Wall -g
OTHERS= amqpFunc.c auxFunctions.c

################################

OBJS=amqpFunc.o auxFunctions.o
MAIN=main.c

################################

.PHONY: clean

all: ep

amqpFunc.o: amqpFunc.h
auxFunctions.o: auxFunctions.h
ep3.o: main.c

ep: ${OBJS} 
		${CC} ${CFLAGS} -o server ${MAIN} ${OTHERS}
		@rm -rf *.o *.dSYM .vscode .DS_Store

clean:
	@rm -rf server