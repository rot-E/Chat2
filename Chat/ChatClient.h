#pragma once

#include "../Error/Error.h"
#include "../Socket/Socket.h"

#include "ChatData.h"

typedef struct ChatClient {
	char *_Host;
	int _Port;
	Socket *_Socket;

	char *_Handle;

	E (* Join)(struct ChatClient *, const char *handle);
	char *(* GetHandle)(struct ChatClient *);
	E (* Post)(struct ChatClient *, const char *message);
	bool (* UpdateExists)(struct ChatClient *);
	StringE (* GetMessage)(struct ChatClient *);
	E (* Quit)(struct ChatClient *);

	void (* Delete)(struct ChatClient *);
} ChatClient;

typedef struct {
	Error *E;
	ChatClient *ChatClient;
} ChatClientE;

extern ChatClientE NewChatClient(const char *host, const int port);
