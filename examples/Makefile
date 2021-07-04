CC		= gcc
CFLAGS	= -g

EXS		= Error TCPClient TCPServer UDPClient UDPServer
PROGS	= TCPEchoServer UDPEchoServer
LIBS	= ../Socket.a ../Error.a

default: ${EXS} ${PROGS}

Error: ../Error.a
TCPClient: EControl.o ${LIBS}
TCPServer: EControl.o ${LIBS}
UDPClient: EControl.o ${LIBS}
UDPServer: EControl.o ${LIBS}

TCPEchoServer: EControl.o ${LIBS}
UDPEchoServer: EControl.o ${LIBS}

${LIBS}:
	make -C ../Socket
	make -C ../Error

clean:
	${RM} ${EXS} ${PROGS} *.o
	make clean -C ../Socket
	make clean -C ../Error
