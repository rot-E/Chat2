#include "Socket.h"

static int32_t_E Accept(Socket_t *self) {
	int32_t sock = accept(self->_Socket, NULL, NULL);

	return (int32_t_E){ Error.New(false), sock };
}

static E Send(Socket_t *self, String_t *message) {
	String_t *str = String.NewFormat("%s%s", message->Unpack(message), Socket.CRLF);

	int32_t result = send(self->_Socket, str->Unpack(str), str->GetLength(str), 0);
		Error_t *err = Error.Build(result == -1, "送信に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return err;

	String.Delete(str);

	return Error.New(false);
}

static String_t_E Receive(Socket_t *self) {
	String_t *str = String.NewN(Socket.DATA_MAX_SIZE);

	int32_t result = recv(self->_Socket, str->Unpack(str), Socket.DATA_MAX_SIZE - 1, 0);
		Error_t *err = Error.Build(result == 0, "切断されました");
		if (!err->IsError(err)) Error.Delete(err); else return (String_t_E){ err };

		err = Error.Build(result == -1, "受信に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (String_t_E){ err };
	String.Reduce(str);

	bool_E crlf = str->EndsWith(str, String.New(Socket.CRLF));
		if (!crlf.E->IsError(crlf.E)) Error.Delete(crlf.E); else return (String_t_E){ crlf.E };
	bool_E lf = str->EndsWithChar(str, String.CHARSET.LF);
		if (!lf.E->IsError(lf.E)) Error.Delete(lf.E); else return (String_t_E){ lf.E };

	uint8_t_ptr_E uE;
	if (crlf.V) {
		/* 区切りがCRLFの場合 */
		uE = str->Substring(str, 0, str->GetLength(str) + 1 - strlen(Socket.CRLF));
	} else if (lf.V) {
		/* 区切りがLFの場合 */
		uE = str->DropLastChar(str);
	}
		if (!uE.E->IsError(uE.E)) Error.Delete(uE.E); else return (String_t_E){ uE.E };

	String.Delete(str);

	return (String_t_E){ Error.New(false), String.New(uE.V) };
}

static void Disconnect(Socket_t *self) {
	close(self->_Socket);
}

static bool UpdateExists(Socket_t *self) {
	fd_set tmp = self->_FDState;
	select(self->_Socket + 1, &tmp, NULL, NULL, &(struct timeval){ .tv_sec = 0, .tv_usec = 0 });

	return FD_ISSET(self->_Socket, &tmp);
}

static E Configure(Socket_t *self, String_t *host, const in_port_t port) {
	// 名前解決
	struct hostent *hent = gethostbyname(host->Unpack(host));
		Error_t *err = Error.Build(hent == NULL, "名前解決に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return err;

	// 接続情報構成
	self->_Addr = (struct sockaddr_in *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.ALLOCATE(sizeof(struct sockaddr_in))
	);
	memset(self->_Addr, 0, sizeof(*self->_Addr));
	self->_Addr->sin_family		= AF_INET;
	self->_Addr->sin_port		= htons(port);
	memcpy((struct in_addr *)(&self->_Addr->sin_addr), hent->h_addr_list[0], hent->h_length);

	return Error.New(false);
}

static E ConfigureBroadcast(Socket_t *self, const in_port_t port) {
	// 接続情報構成
	self->_Addr = (struct sockaddr_in *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.ALLOCATE(sizeof(struct sockaddr_in))
	);
	memset(self->_Addr, 0, sizeof(*self->_Addr));
	self->_Addr->sin_family			= AF_INET;
	self->_Addr->sin_port			= htons(port);
	self->_Addr->sin_addr.s_addr	= htonl(INADDR_BROADCAST);

	// ソケットの設定
	self->_BroadcastSwitch = 1;

	int32_t result = setsockopt(self->_Socket, SOL_SOCKET, SO_BROADCAST, (void *)(&self->_BroadcastSwitch), sizeof(self->_BroadcastSwitch));
		Error_t *err = Error.Build(result == -1, "ソケットのブロードキャスト可能化に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return err;

	return Error.New(false);
}

static String_t *GetDestIPAddr(Socket_t *self) {
	return String.New(inet_ntoa(self->_Addr->sin_addr));
}

static in_port_t GetDestPort(Socket_t *self) {
	return ntohs(self->_Addr->sin_port);
}

static E Cast(Socket_t *self, String_t *message) {
	int32_t result = sendto(self->_Socket, message->Unpack(message), message->GetLength(message), 0, (struct sockaddr *)(self->_Addr), sizeof(*self->_Addr));
		Error_t *err = Error.Build(result == -1, "送信に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return err;

	return Error.New(false);
}

static E Broadcast(Socket_t *self, String_t *message) {
	Error_t *err = self->Cast(self, message);
	if (!err->IsError(err)) Error.Delete(err); else return err;

	return Error.New(false);
}

static String_t_E Collect(Socket_t *self) {
	String_t *str = String.NewN(Socket.DATA_MAX_SIZE);
	int32_t result = recvfrom(self->_Socket, str->Unpack(str), str->GetLength(str) - 1, 0, (struct sockaddr *)(self->_Addr),
		({
			socklen_t tmp = sizeof(*self->_Addr);
			&tmp;
		})
	);
		Error_t *err = Error.Build(result == -1, "受信に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (String_t_E){ err };;

	uint8_t_ptr_E concat = str->ConcatChar(str, String.CHARSET.Null);
		if (!concat.E->IsError(concat.E)) Error.Delete(concat.E); else return (String_t_E){ concat.E };;

	return (String_t_E){ Error.New(false), String.New(concat.V) };
}


static Socket_t *New(const int32_t socket) {
	Socket_t *sock = (Socket_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.ALLOCATE(sizeof(Socket_t)));

	sock->Accept				= Accept;
	sock->Send					= Send;
	sock->Receive				= Receive;
	sock->Disconnect			= Disconnect;
	sock->UpdateExists			= UpdateExists;
	sock->Configure				= Configure;
	sock->ConfigureBroadcast	= ConfigureBroadcast;
	sock->GetDestIPAddr			= GetDestIPAddr;
	sock->GetDestPort			= GetDestPort;
	sock->Cast					= Cast;
	sock->Broadcast				= Broadcast;
	sock->Collect				= Collect;

	sock->_Socket				= (socket >= 3)? socket : 0;
	sock->_Addr					= NULL;
	sock->_BroadcastSwitch		= 0;

	// ソケット監視設定
	if (socket >= 3) {
		FD_ZERO(&sock->_FDState);
		FD_SET(sock->_Socket, &sock->_FDState);
	}

	return sock;
}

static Socket_t_E NewTCPClient(String_t *serverHost, const in_port_t serverPort) {
	Socket_t *sock = Socket.New(-1);

	// ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_STREAM, 0);
		Error_t *err = Error.Build(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// 名前解決
	struct hostent *hent = gethostbyname(serverHost->Unpack(serverHost));
		err = Error.Build(hent == NULL, "名前解決に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// 接続情報構成
	sock->_Addr = (struct sockaddr_in *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.ALLOCATE(sizeof(struct sockaddr_in)));
	memset(sock->_Addr, 0, sizeof(*sock->_Addr));
	sock->_Addr->sin_family		= AF_INET;
	sock->_Addr->sin_port		= htons(serverPort);
	memcpy(&sock->_Addr->sin_addr, hent->h_addr_list[0], hent->h_length);

	// 接続
	int32_t result = connect(sock->_Socket, (struct sockaddr *)(sock->_Addr), sizeof(*sock->_Addr));
		err = Error.Build(result == -1, "接続に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// ソケット監視設定
	FD_ZERO(&sock->_FDState);
	FD_SET(sock->_Socket, &sock->_FDState);

	return (Socket_t_E){ Error.New(false), sock };
}

static Socket_t_E NewTCPServer(const in_port_t listenPort) {
	Socket_t *sock = Socket.New(-1);

	// 待受用ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_STREAM, 0);
		Error_t *err = Error.Build(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// 接続情報構成
	sock->_Addr = (struct sockaddr_in *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.ALLOCATE(sizeof(struct sockaddr_in)));
	memset(sock->_Addr, 0, sizeof(*sock->_Addr));

	sock->_Addr->sin_family = AF_INET;
	sock->_Addr->sin_port = htons(listenPort);
	sock->_Addr->sin_addr.s_addr = htonl(INADDR_ANY);

	// 名前を付与
	int32_t result = bind(sock->_Socket, (struct sockaddr *)(sock->_Addr), sizeof(*sock->_Addr));
		err = Error.Build(result == -1, "名前付与に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// 待受開始
	result = listen(sock->_Socket, 5);
		err = Error.Build(result == -1, "待受開始できず");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	return (Socket_t_E){ Error.New(false), sock };
}

static Socket_t_E NewUDPClient() {
	Socket_t *sock = Socket.New(-1);

	// ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_DGRAM, 0);
		Error_t *err = Error.Build(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// ソケット監視設定
	FD_ZERO(&sock->_FDState);
	FD_SET(sock->_Socket, &sock->_FDState);

	return (Socket_t_E){ Error.New(false), sock };
}

static Socket_t_E NewUDPServer(const in_port_t listenPort) {
	Socket_t *sock = Socket.New(-1);

	// ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_DGRAM, 0);
		Error_t *err = Error.Build(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// 接続情報構成
	sock->_Addr = (struct sockaddr_in *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.ALLOCATE(sizeof(struct sockaddr_in)));
	memset(sock->_Addr, 0, sizeof(*sock->_Addr));
	sock->_Addr->sin_family			= AF_INET;
	sock->_Addr->sin_port			= htons(listenPort);
	sock->_Addr->sin_addr.s_addr	= htonl(INADDR_ANY);

	// 名前を付与
	int32_t result = bind(sock->_Socket, (struct sockaddr *)(sock->_Addr), sizeof(*sock->_Addr));
		err = Error.Build(result == -1, "名前付与に失敗");
		if (!err->IsError(err)) Error.Delete(err); else return (Socket_t_E){ err };

	// ソケット監視設定
	FD_ZERO(&sock->_FDState);
	FD_SET(sock->_Socket, &sock->_FDState);

	return (Socket_t_E){ Error.New(false), sock };
}

static void Release(Socket_t *sock) {
	free(sock->_Addr);
}

static void Delete(Socket_t *sock) {
	Socket.Release(sock);
	free(sock);
}


_Socket Socket = {
	.CRLF				= "\r\n",
	.DATA_MAX_SIZE		= 1000,

	.New				= New,
	.NewTCPClient		= NewTCPClient,
	.NewTCPServer		= NewTCPServer,
	.NewUDPClient		= NewUDPClient,
	.NewUDPServer		= NewUDPServer,

	.Release			= Release,
	.Delete				= Delete,
};
