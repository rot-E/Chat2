#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "../Error/Error.h"
#include "../Error/EType.h"
#include "../String/String.h"

typedef struct Socket_t {
	int32_t _Socket;
	fd_set _FDState;

	struct sockaddr_in *_Addr;
	int32_t _BroadcastSwitch;

	/* TCP */
	int32_t_E (* Accept)(struct Socket_t *);

	E (* Send)(struct Socket_t *, String_t *message);
	String_t_E (* Receive)(struct Socket_t *);

	void (* Disconnect)(struct Socket_t *);

	/* Common */
	bool (* UpdateExists)(struct Socket_t *);

	/* UDP */
	E (* Configure)(struct Socket_t *, String_t *serverHost, const in_port_t serverPort);
	E (* ConfigureBroadcast)(struct Socket_t *, const in_port_t serverPort);

	String_t *(* GetDestIPAddr)(struct Socket_t *);
	in_port_t (* GetDestPort)(struct Socket_t *);

	E (* Cast)(struct Socket_t *, String_t *message);
	E (* Broadcast)(struct Socket_t *, String_t *message);
	String_t_E (* Collect)(struct Socket_t *);
} Socket_t;

typedef struct {
	Error_t *E;
	Socket_t *V;
} Socket_t_E;

typedef struct {
	Failure_t *F;
	Socket_t *V;
} Socket_t_F;


typedef struct {
	uint8_t *CRLF;
	int32_t DATA_MAX_SIZE;

	// TODO: 内部の処理の共通化
	// TODO: DATA_MAX周り? 辻褄合わせ -1など 要確認
	Socket_t *(* New)(const int32_t socket);
	Socket_t_E (* NewTCPClient)(String_t *serverHost, const in_port_t serverPort);
	Socket_t_E (* NewTCPServer)(const in_port_t listenPort);
	Socket_t_E (* NewUDPClient)();
	Socket_t_E (* NewUDPServer)(const in_port_t listenPort);
	void (* Release)(Socket_t *);
	void (* Delete)(Socket_t *);
} _Socket;

extern _Socket Socket;
