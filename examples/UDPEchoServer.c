#include "EControl.h"

void main() {
	Socket *svr = UnwrapE(NewUDPServerSocket(10001));

	for (;;)
		if (svr->UpdateExists(svr))
			InspectE(svr->Cast(svr, UnwrapE(svr->Collect(svr)) ));
}
