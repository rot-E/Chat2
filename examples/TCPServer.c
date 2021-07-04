/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc TCPServer.c EControl.c ../Socket.a ../Error.a -o TCPServer
// ./TCPServer

#include <stdio.h>

#include "../Error/Error.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket *listener = UnwrapE(NewTCPServerSocket(10002));
	Socket *svr = UnwrapE(NewSocket( UnwrapE(listener->Accept(listener)) ));
	printf("[Accepted]\n"); fflush(stdin);

	for (;;) {
		if (svr->UpdateExists(svr)) {
			char *msg = UnwrapE(svr->Receive(svr));
			printf("> %s\n", msg); fflush(stdin);

			char str[DATA_MAX_SIZE];
			sprintf(str, "Received: %s", msg);
			InspectE(svr->Send(svr, str));
			printf("Sent: %s\n", str); fflush(stdin);
		}
	}

	svr->Delete(svr);
	listener->Delete(listener);
}
