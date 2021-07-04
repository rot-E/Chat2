#pragma once

#include "Error/Error.h"
#include "Error/EType.h"
#include "Socket/Socket.h"
#include "Chat/ChatClient.h"

extern void InspectE(E);

#define UnwrapE(X) _Generic((X),		\
	intE:			UnwrapIntE,			\
	StringE:		UnwrapStringE,		\
	SocketE:		UnwrapSocketE,		\
	ChatClientE:	UnwrapChatClientE	\
)(X)

extern int UnwrapIntE(intE);
extern char *UnwrapStringE(StringE);
extern Socket *UnwrapSocketE(SocketE);
extern ChatClient *UnwrapChatClientE(ChatClientE);
