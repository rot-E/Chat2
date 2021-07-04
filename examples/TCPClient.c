/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc TCPClient.c EControl.c ../Socket.a ../Error.a -o TCPClient
// ./TCPClient

#include <stdio.h>

#include "../Error/Error.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket *cli = UnwrapE(NewTCPClientSocket("localhost", 10002));

	for (int i = 0;; i++) {
		char str[DATA_MAX_SIZE];
		sprintf(str, "Hello %d.", i);

		InspectE(cli->Send(cli, str));
		printf("Sent: %s\n", str);

		usleep(100 * 1000);

		if (cli->UpdateExists(cli)) {
			char *msg = UnwrapE(cli->Receive(cli));
			printf("> %s\n", msg); fflush(stdin);
		}

		if (i == 10) {
			cli->Disconnect(cli);
			break;
		}
	}

	cli->Delete(cli);
}
