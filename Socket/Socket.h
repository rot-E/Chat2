#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "../Error/Error.h"
#include "../Error/EType.h"

extern const char CRLF[];
extern const int DATA_MAX_SIZE;

typedef struct Socket {
	int _Socket;
	fd_set _FDState;

	struct sockaddr_in *_Addr;
	int _BroadcastSwitch;

	/* TCP */
	intE (* Accept)(struct Socket *);

	E (* Send)(struct Socket *, const char *message);
	StringE (* Receive)(struct Socket *);

	void (* Disconnect)(struct Socket *);

	/* Common */
	bool (* UpdateExists)(struct Socket *);

	/* UDP */
	E (* Configure)(struct Socket *, const char *serverHost, const in_port_t serverPort);
	E (* ConfigureBroadcast)(struct Socket *, const in_port_t serverPort);

	char *(* GetDestIPAddr)(struct Socket *);
	in_port_t (* GetDestPort)(struct Socket *);

	E (* Cast)(struct Socket *, const char *message);
	E (* Broadcast)(struct Socket *, const char *message);
	StringE (* Collect)(struct Socket *);

	void (* Delete)(struct Socket *);
} Socket;

typedef struct {
	Error *E;
	Socket *Socket;
} SocketE;

extern SocketE NewSocket(const int socket);
extern SocketE NewTCPClientSocket(const char *serverHost, const in_port_t serverPort);
extern SocketE NewTCPServerSocket(const in_port_t listenPort);
extern SocketE NewUDPClientSocket();
extern SocketE NewUDPServerSocket(const in_port_t listenPort);
