CFLAGS	= -g

LIBS	= Chat.a Socket.a Error.a

default: Main

Main: EControl.o ${LIBS}

${LIBS}:
	make -C Chat
	make -C Socket
	make -C Error

clean:
	${RM} Main *.o
	make clean -C Chat
	make clean -C Socket
	make clean -C Error
