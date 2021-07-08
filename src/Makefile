CFLAGS	= -g

LIBS	= Chat.a Socket.a Console.a String.a Error.a SPEC.a

default: Main

Main: EControl.o ${LIBS}

${LIBS}:
	make -C Chat
	make -C Socket
	make -C Console
	make -C String
	make -C Error
	make -C SPEC

clean:
	${RM} Main *.o
	make clean -C Chat
	make clean -C Socket
	make clean -C Console
	make clean -C String
	make clean -C Error
	make clean -C SPEC
