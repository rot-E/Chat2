#include "EControl.h"

void main() {
	Socket *listener = UnwrapE(NewTCPServerSocket(10001));
	Socket *svr = UnwrapE(NewSocket( UnwrapE(listener->Accept(listener)) ));

	for (;;)
		if (svr->UpdateExists(svr))
			InspectE(svr->Send(svr, UnwrapE(svr->Receive(svr))));
}
