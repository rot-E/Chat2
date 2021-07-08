/**
 * Socket.aの使用例
 */

// make -C ../Socket
// make -C ../Error
// gcc UDPServer.c EControl.c ../Socket.a ../Error.a -o UDPServer
// ./UDPServer

#include <stdio.h>

#include "../Console/Console.h"
#include "../Socket/Socket.h"

#include "EControl.h"

void main() {
	Socket_t *svr = UnwrapE(Socket.NewUDPServer(10001));

	for (;;) {
		if (svr->UpdateExists(svr)) {
			String_t *msg = UnwrapE(svr->Collect(svr));
			Console.WriteLine(
				String.NewFormat(
					"[%s, %d]> %s",
					({
						String_t *ip = svr->GetDestIPAddr(svr);
						ip->Unpack(ip);
					}),
					svr->GetDestPort(svr),
					msg->Unpack(msg)
				)
			);

			InspectE(svr->Cast(svr, String.New("Received.")));
		}
	}

	Socket.Delete(svr);
}
