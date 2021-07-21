#include "Chat.h"

static void Setup() {
	Chat.Exception signal;
	Chat.HandleException signal;
	Chat.NetworkException signal;
}

_Chat Chat = {
	.Setup					= Setup,

	.HANDLE_LENGTH_MAX		= 15,
};
