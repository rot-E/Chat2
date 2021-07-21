CFLAGS	= -I./include -g
LIBS	= -L. -lNeoC -lpthread

default: Main

Main: Chat.a
	gcc ${CFLAGS} Main.c Chat.a ${LIBS} -o Main

Chat.a:
	make -C Chat

clean:
	${RM} Main *.o
	make clean -C Chat
	${RM} -r NeoC*
