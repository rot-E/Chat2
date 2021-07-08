/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc TCPServer.c EControl.c ../Socket.a ../Error.a -o TCPServer
// ./TCPServer

#include <stdio.h>

#include "../Error/Error.h"
#include "../Console/Console.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket_t *listener = UnwrapE(Socket.NewTCPServer(10001));
	Socket_t *svr = Socket.New(UnwrapE(listener->Accept(listener)));
	Console.WriteLine(String.New("[Accepted]"));
	Console.WriteLine(String.New(""));

	for (;;) {
		if (svr->UpdateExists(svr)) {
			String_t *msg = UnwrapE(svr->Receive(svr));
			Console.WriteLine(String.NewFormat("[Client]> %s", msg->Unpack(msg)));

			String_t *str = String.NewFormat("Received: <%s>", msg->Unpack(msg));
			InspectE(svr->Send(svr, str));
			Console.WriteLine(String.NewFormat("[Server]> %s", str->Unpack(str)));

			String.Delete(msg);
			String.Delete(str);
		}
	}

	Socket.Delete(svr);
	Socket.Delete(listener);
}
