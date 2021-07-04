#include "EControl.h"

extern void InspectE(E E) {
	E->ExitIfError(E);
	E->Delete(E);
}

extern int UnwrapIntE(intE ret) {
	InspectE(ret.E);
	return ret.Int;
}

extern char *UnwrapStringE(StringE ret) {
	InspectE(ret.E);
	return ret.String;
}

extern Socket *UnwrapSocketE(SocketE ret) {
	InspectE(ret.E);
	return ret.Socket;
}

extern ChatClient *UnwrapChatClientE(ChatClientE ret) {
	InspectE(ret.E);
	return ret.ChatClient;
}
