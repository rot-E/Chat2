/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc UDPServer.c EControl.c ../Socket.a ../Error.a -o UDPServer
// ./UDPServer

#include <stdio.h>

#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket *svr = UnwrapE(NewUDPServerSocket(10001));

	for (;;) {
		if (svr->UpdateExists(svr)) {
			char *msg = UnwrapE(svr->Collect(svr));
			printf("[%s, %d]> %s\n", svr->GetDestIPAddr(svr), svr->GetDestPort(svr), msg); fflush(stdin);

			InspectE(svr->Cast(svr, "Received."));
		}
	}

	svr->Delete(svr);
}
