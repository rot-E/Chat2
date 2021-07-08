#include "EControl.h"

void main() {
	Socket_t *svr = UnwrapE(Socket.NewUDPServer(10001));

	for (;;)
		if (svr->UpdateExists(svr))
			InspectE(svr->Cast(svr, UnwrapE(svr->Collect(svr)) ));
}
