AR = ar -cq
CFLAGS = -g

LIB = ../Chat.a
OBJS = ChatClient.o ChatData.o

default: ${LIB}

${LIB}: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ${LIB}
