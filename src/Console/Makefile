AR = ar -cq
CFLAGS = -g

LIB = ../Console.a
OBJS = Console.o

default: ${LIB}

${LIB}: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ${LIB}
