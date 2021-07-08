#pragma once

#include <stdint.h>

#include "../Error/Error.h"
#include "../Error/Failure.h"
#include "../String/String.h"
#include "../Socket/Socket.h"

#include "CHAT.h"

typedef struct ChatClient_t {
	String_t *_Host;
	int32_t _Port;
	Socket_t *_Socket;
	String_t *_Handle;

	E (* Join)(struct ChatClient_t *, String_t *handle);
	String_t *(* GetHandle)(struct ChatClient_t *);
	E (* Post)(struct ChatClient_t *, String_t *message);
	bool (* UpdateExists)(struct ChatClient_t *);
	String_t_E (* GetMessage)(struct ChatClient_t *);
	E (* Quit)(struct ChatClient_t *);
} ChatClient_t;

typedef struct {
	Error_t *E;
	ChatClient_t *V;
} ChatClient_t_E;

typedef struct {
	Failure_t *F;
	ChatClient_t *V;
} ChatClient_t_F;


typedef struct {
	ChatClient_t *(* New)(String_t *host, const in_port_t port);
	void (* Release)(ChatClient_t *);
	void (* Delete)(ChatClient_t *);
} _ChatClient;

extern _ChatClient ChatClient;
