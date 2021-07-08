AR = ar -cq
CFLAGS = -g

LIB = ../SPEC.a
OBJS = ESCAPE_SEQUENCE.o 

default: ${LIB}

${LIB}: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ${LIB}
