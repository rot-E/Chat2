/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc UDPClient.c EControl.c ../Socket.a ../Error.a -o UDPClient
// ./UDPClient

#include <stdio.h>

#include "../Error/Error.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket *cli = UnwrapE(NewUDPClientSocket());
	InspectE(cli->Configure(cli, "localhost", 10001));

	for (int i = 0;; i++) {
		char str[DATA_MAX_SIZE];
		sprintf(str, "Hello %d.", i);
		InspectE(cli->Cast(cli, str));
		printf("Casted: %s\n", str);
		usleep(100 * 1000);

		if (cli->UpdateExists(cli)) {
			char *msg = UnwrapE(cli->Collect(cli));
			printf("[%s, %d]> %s\n", cli->GetDestIPAddr(cli), cli->GetDestPort(cli), msg); fflush(stdin);
		}

		if (i == 10) {
			cli->Disconnect(cli);
			break;
		}
	}
	cli->Delete(cli);


	/* Broadcast */
	Socket *cli2 = UnwrapE(NewUDPClientSocket());
	InspectE(cli2->ConfigureBroadcast(cli2, 10001));
	InspectE(cli2->Broadcast(cli2, "HELO"));
	printf("Broadcasted\n");

	cli2->Delete(cli2);
}
