#pragma once

#include <thread.h>
#include <sys/select.h>

#include "../Error/Error.h"
#include "CHAT.h"

typedef struct ChatServer_t {
	int _Port;

	Error * (* Start)(struct ChatServer *);
} ChatServer;

typedef struct {
	Error *E;
	ChatServer *V;
} ChatServer_t_E;

typedef struct {
	Failure *F;
	ChatServer *V;
} ChatServer_t_F;

typedef struct {

} _ChatServer;

extern _ChatServer ChatServer;
