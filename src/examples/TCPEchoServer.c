#include "EControl.h"

void main() {
	Socket_t *listener = UnwrapE(Socket.NewTCPServer(10001));
	Socket_t *svr = Socket.New(UnwrapE(listener->Accept(listener)));

	for (;;)
		if (svr->UpdateExists(svr))
			InspectE(svr->Send(svr, UnwrapE(svr->Receive(svr))));
}
