#include "ChatServer.h"

static void Start(ChatServer_t *self) {
}

static ChatServer_t *New(const in_port_t port) {
	ChatServer_t *svr = (ChatServer_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.NewADT(sizeof(ChatServer_t)));

	svr->Start		= Start;

	svr->_Port		= port;
	svr->_Socket	= NULL;

	return svr;
}

static void Release(ChatServer_t *svr) {
	Socket.Delete(svr->_Socket);
}

static void Delete(ChatServer_t *svr) {
	ChatServer.Release(svr);
	free(svr);
}


_ChatServer ChatServer = {
	.New			= New,	
	.Release		= Release,
	.Delete			= Delete,
};
