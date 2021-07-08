#include "EControl.h"

extern void InspectE(E err) {
	err->ExitIfError(err);
	Error.Delete(err);
}

extern bool UnwrapBoolE(bool_E ret) {
	InspectE(ret.E);
	return ret.V;
}

extern int32_t UnwrapInt32E(int32_t_E ret) {
	InspectE(ret.E);
	return ret.V;
}

extern uint8_t UnwrapUInt8E(uint8_t_E ret) {
	InspectE(ret.E);
	return ret.V;
}

extern uint8_t *UnwrapUInt8PtrE(uint8_t_ptr_E ret) {
	InspectE(ret.E);
	return ret.V;
}

extern String_t *UnwrapStringE(String_t_E ret) {
	InspectE(ret.E);
	return ret.V;
}

extern Socket_t *UnwrapSocketE(Socket_t_E ret) {
	InspectE(ret.E);
	return ret.V;
}
