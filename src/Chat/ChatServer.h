#pragma once

#include <threads.h>
#include <sys/select.h>

#include "../SPEC/ESCAPE_SEQUENCE.h"
#include "../Error/Error.h"
#include "../Error/Failure.h"
#include "../String/String.h"
#include "../Console/Console.h"
#include "../Socket/Socket.h"

#include "CHAT.h"

typedef struct ChatServer_t {
	in_port_t _Port;
	Socket_t *_Socket;

	void (* Start)(struct ChatServer_t *);
} ChatServer_t;

typedef struct {
	Error_t *E;
	ChatServer_t *V;
} ChatServer_t_E;

typedef struct {
	Failure_t *F;
	ChatServer_t *V;
} ChatServer_t_F;

typedef struct {
	ChatServer_t *(* New)(const in_port_t port);
	void (* Release)(ChatServer_t *);
	void (* Delete)(ChatServer_t *);
} _ChatServer;

extern _ChatServer ChatServer;
