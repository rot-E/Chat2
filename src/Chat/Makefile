AR		= ar -cq
CFLAGS	= -I../include -g

OBJS = ChatClient.o ChatServer.o Chat.o

default: ../Chat.a

../Chat.a: ${OBJS}
	${RM} $@
	${AR} $@ ${OBJS}

clean:
	${RM} ${OBJS}
	${RM} ../Chat.a
