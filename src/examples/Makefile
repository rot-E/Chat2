CC		= gcc
CFLAGS	= -g

EXS		= Error Failure String Console TCPClient TCPServer UDPClient UDPServer
PROGS	= TCPEchoServer UDPEchoServer
LIBS	= ../Socket.a ../Console.a ../String.a ../Error.a ../SPEC.a

default: ${EXS} ${PROGS}

Error Failure: ../Error.a ../SPEC.a
String: EControl.o ../String.a ../Error.a ../SPEC.a
Console: ../Console.a ../String.a ../Error.a ../SPEC.a
TCPClient TCPServer UDPClient UDPServer TCPEchoServer UDPEchoServer: EControl.o ${LIBS}

${LIBS}:
	make -C ../Socket
	make -C ../Console
	make -C ../String
	make -C ../Error
	make -C ../SPEC

clean:
	${RM} ${EXS} ${PROGS} *.o
	make clean -C ../Socket
	make clean -C ../Console
	make clean -C ../String
	make clean -C ../Error
	make clean -C ../SPEC
