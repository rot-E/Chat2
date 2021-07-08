AR = ar -cq
CFLAGS = -g

LIB = ../Socket.a
OBJS = Socket.o

default: ${LIB}

${LIB}: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ${LIB}
