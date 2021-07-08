#pragma once

#include <pthread.h>
#include <sys/select.h>

#include "../Error/Error.h"
#include "ChatData.h"

#include "../mynet/mynet.h"

typedef struct ChatServer {
	int _Port;

	Error * (* Start)(struct ChatServer *);
} ChatServer;

typedef struct {
	Error *E;
	ChatServer *ChatServer;
} Return_NewChatServer;

extern Return_NewChatServer * NewChatServer(const int port);
