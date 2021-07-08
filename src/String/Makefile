AR = ar -cq
CFLAGS = -g

LIB = ../String.a
OBJS = String.o

default: ${LIB}

${LIB}: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ${LIB}
