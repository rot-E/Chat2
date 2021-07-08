AR = ar -cq
CFLAGS = -g

LIB = ../Error.a
OBJS = Failure.o Error.o

default: ${LIB}

${LIB}: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ${LIB}
