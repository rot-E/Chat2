/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc TCPClient.c EControl.c ../Socket.a ../Error.a -o TCPClient
// ./TCPClient

#include <stdio.h>

#include "../Error/Error.h"
#include "../String/String.h"
#include "../Console/Console.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket_t *cli = UnwrapE(Socket.NewTCPClient(String.New("localhost"), 10001));

	for (int32_t i = 0;; i++) {
		String_t *str = String.NewFormat("Hello %d.", i);

		InspectE(cli->Send(cli, str));
		Console.WriteLine(String.NewFormat("[Client]> %s", str->Unpack(str)));

		usleep(100 * 1000);

		if (cli->UpdateExists(cli)) {
			String_t *msg = UnwrapE(cli->Receive(cli));
			Console.WriteLine( String.NewFormat("[Server]> %s", msg->Unpack(msg)) );
		}

		if (i == 10) {
			cli->Disconnect(cli);
			break;
		}
	}

	Socket.Delete(cli);
}
