/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc UDPClient.c EControl.c ../Socket.a ../Error.a -o UDPClient
// ./UDPClient

#include <stdio.h>

#include "../Error/Error.h"
#include "../Console/Console.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket_t *cli = UnwrapE(Socket.NewUDPClient());
	InspectE(cli->Configure(cli, String.New("localhost"), 10001));

	for (int i = 0;; i++) {
		String_t *str = String.NewFormat("Hello %d.", i);
		InspectE(cli->Cast(cli, str));
		Console.WriteLine(String.NewFormat("Casted: %s", str->Unpack(str)));
		usleep(100 * 1000);

		if (cli->UpdateExists(cli)) {
			String_t *msg = UnwrapE(cli->Collect(cli));
			String_t *ip = cli->GetDestIPAddr(cli);
			Console.WriteLine(String.NewFormat("[%s, %d]> %s", ip->Unpack(ip), cli->GetDestPort(cli), msg->Unpack(msg)));
		}

		if (i == 10) {
			cli->Disconnect(cli);
			break;
		}
	}
	Socket.Delete(cli);


	/* Broadcast */
	Socket_t *cli2 = UnwrapE(Socket.NewUDPClient());
	InspectE(cli2->ConfigureBroadcast(cli2, 10001));
	InspectE(cli2->Broadcast(cli2, String.New("HELO")));
	Console.WriteLine(String.New("Broadcasted."));

	Socket.Delete(cli2);
}
