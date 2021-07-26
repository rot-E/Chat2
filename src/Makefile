CC		= gcc
CFLAGS	= -g
LIBS	= -I./include -L. -lNeoC -lpthread

default: Main

Main: Chat.a
	${CC} ${CFLAGS} Main.c Chat.a ${LIBS} -o Main

Chat.a:
	make -C Chat

clean:
	${RM} Main *.o
	make clean -C Chat
