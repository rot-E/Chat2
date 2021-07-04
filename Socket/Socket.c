#include "Socket.h"

const char CRLF[]			= "\r\n";
const int DATA_MAX_SIZE		= 500;

static intE Accept(Socket *self) {
	int sock = accept(self->_Socket, NULL, NULL);

	return (intE){ NewError(false), sock };
}

static E Send(Socket *self, const char *message) {
	char str[DATA_MAX_SIZE];
	sprintf(str, "%s%s", message, CRLF);

	int result = send(self->_Socket, str, strlen(str), 0);
		Error *err = BuildError(result == -1, "送信に失敗");
		if (!err->IsError(err)) err->Delete(err); else return err;

	return NewError(false);
}

static StringE Receive(Socket *self) {
	char *str = (char *)(malloc(sizeof(char) * DATA_MAX_SIZE));

	int result = recv(self->_Socket, str, DATA_MAX_SIZE, 0);
		Error *err = BuildError(result == 0, "切断されました");
		if (!err->IsError(err)) err->Delete(err); else return (StringE){ err };

		err = BuildError(result == -1, "受信に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (StringE){ err };

	if (str[strlen(str) - 2] == '\r') {
		/* 区切りがCRLFの場合 */
		str[strlen(str) - 1] = '\0'; // 末尾LF削除
		str[strlen(str) - 1] = '\0'; // 末尾CR削除
	} else if (str[strlen(str) - 1] == '\n') {
		/* 区切りがLFの場合 */
		str[strlen(str) - 1] = '\0'; // 末尾LF削除
	}

	return (StringE){ NewError(false), str };
}

static void Disconnect(Socket *self) {
	close(self->_Socket);
}

static bool UpdateExists(Socket *self) {
	fd_set tmp = self->_FDState;
	select(self->_Socket + 1, &tmp, NULL, NULL, &(struct timeval){ .tv_sec = 0, .tv_usec = 0 });

	return FD_ISSET(self->_Socket, &tmp);
}

static E Configure(Socket *self, const char *host, const in_port_t port) {
	// 名前解決
	struct hostent *hent = gethostbyname(host);
		Error *err = BuildError(hent == NULL, "名前解決に失敗");
		if (!err->IsError(err)) err->Delete(err); else return err;

	// 接続情報構成
	self->_Addr = (struct sockaddr_in *)(malloc(sizeof(struct sockaddr_in)));
	memset(self->_Addr, 0, sizeof(*self->_Addr));
	self->_Addr->sin_family		= AF_INET;
	self->_Addr->sin_port		= htons(port);
	memcpy((struct in_addr *)(&self->_Addr->sin_addr), hent->h_addr_list[0], hent->h_length);

	return NewError(false);
}

static E ConfigureBroadcast(Socket *self, const in_port_t port) {
	// 接続情報構成
	self->_Addr = (struct sockaddr_in *)(malloc(sizeof(struct sockaddr_in)));
	memset(self->_Addr, 0, sizeof(*self->_Addr));
	self->_Addr->sin_family			= AF_INET;
	self->_Addr->sin_port			= htons(port);
	self->_Addr->sin_addr.s_addr	= htonl(INADDR_BROADCAST);

	// ソケットの設定
	self->_BroadcastSwitch = 1;

	int result = setsockopt(self->_Socket, SOL_SOCKET, SO_BROADCAST, (void *)(&self->_BroadcastSwitch), sizeof(self->_BroadcastSwitch));
		Error *err = BuildError(result == -1, "ソケットのブロードキャスト可能化に失敗");
		if (!err->IsError(err)) err->Delete(err); else return err;

	return NewError(false);
}

static char *GetDestIPAddr(Socket *self) {
	return inet_ntoa(self->_Addr->sin_addr);
}

static in_port_t GetDestPort(Socket *self) {
	return ntohs(self->_Addr->sin_port);
}

static E Cast(Socket *self, const char *message) {
	int result = sendto(self->_Socket, message, strlen(message), 0, (struct sockaddr *)(self->_Addr), sizeof(*self->_Addr));
		Error *err = BuildError(result == -1, "送信に失敗");
		if (!err->IsError(err)) err->Delete(err); else return err;

	return NewError(false);
}

static E Broadcast(Socket *self, const char *message) {
	Error *err = self->Cast(self, message);
	if (!err->IsError(err)) err->Delete(err); else return err;

	return NewError(false);
}

static StringE Collect(Socket *self) {
	char *str = (char *)(malloc(sizeof(char) * DATA_MAX_SIZE));

	int result = recvfrom(self->_Socket, str, strlen(str) - 1, 0, (struct sockaddr *)(self->_Addr),
		({
			socklen_t tmp = sizeof(*self->_Addr);
			&tmp;
		})
	);
		Error *err = BuildError(result == -1, "受信に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (StringE){ err };;
	str[strlen(str)] = '\0';

	return (StringE){ NewError(false), str };
}

static void Delete(Socket *self) {
	close(self->_Socket);

	free(self->_Addr);
	free(self);
}

extern SocketE NewSocket(const int socket) {
	Socket *sock = (Socket *)(malloc(sizeof(Socket)));
		Error *err = BuildError(sock == NULL, "動的メモリ確保に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

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

	sock->Delete				= Delete;

	sock->_Socket				= (socket >= 3)? socket : 0;

	sock->_Addr					= NULL;
	sock->_BroadcastSwitch		= 0;

	// ソケット監視設定
	if (socket >= 3) {
		FD_ZERO(&sock->_FDState);
		FD_SET(sock->_Socket, &sock->_FDState);
	}

	return (SocketE){ NewError(false), sock };
}

extern SocketE NewTCPClientSocket(const char *serverHost, const in_port_t serverPort) {
	SocketE sockE = NewSocket(-1);
		if (!sockE.E->IsError(sockE.E)) sockE.E->Delete(sockE.E); else return (SocketE){ sockE.E };
	Socket *sock = sockE.Socket;

	// ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_STREAM, 0);
		Error *err = BuildError(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// 名前解決
	struct hostent *hent = gethostbyname(serverHost);
		err = BuildError(hent == NULL, "名前解決に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// 接続情報構成
	sock->_Addr = (struct sockaddr_in *)(malloc(sizeof(struct sockaddr_in)));
	memset(sock->_Addr, 0, sizeof(*sock->_Addr));
	sock->_Addr->sin_family		= AF_INET;
	sock->_Addr->sin_port		= htons(serverPort);
	memcpy(&sock->_Addr->sin_addr, hent->h_addr_list[0], hent->h_length);

	// 接続
	int result = connect(sock->_Socket, (struct sockaddr *)(sock->_Addr), sizeof(*sock->_Addr));
		err = BuildError(result == -1, "接続に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// ソケット監視設定
	FD_ZERO(&sock->_FDState);
	FD_SET(sock->_Socket, &sock->_FDState);

	return (SocketE){ NewError(false), sock };
}

extern SocketE NewTCPServerSocket(const in_port_t listenPort) {
	SocketE sockE = NewSocket(-1);
		if (!sockE.E->IsError(sockE.E)) sockE.E->Delete(sockE.E); else return (SocketE){ sockE.E };
	Socket *sock = sockE.Socket;

	// 待受用ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_STREAM, 0);
		Error *err = BuildError(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// 接続情報構成
	sock->_Addr = (struct sockaddr_in *)(malloc(sizeof(struct sockaddr_in)));
	memset(sock->_Addr, 0, sizeof(*sock->_Addr));

	sock->_Addr->sin_family = AF_INET;
	sock->_Addr->sin_port = htons(listenPort);
	sock->_Addr->sin_addr.s_addr = htonl(INADDR_ANY);

	// 名前を付与
	int result = bind(sock->_Socket, (struct sockaddr *)(sock->_Addr), sizeof(*sock->_Addr));
		err = BuildError(result == -1, "名前付与に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// 待受開始
	result = listen(sock->_Socket, 5);
		err = BuildError(result == -1, "待受開始できず");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	return (SocketE){ NewError(false), sock };
}

extern SocketE NewUDPClientSocket() {
	SocketE sockE = NewSocket(-1);
		if (!sockE.E->IsError(sockE.E)) sockE.E->Delete(sockE.E); else return (SocketE){ sockE.E };
	Socket *sock = sockE.Socket;

	// ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_DGRAM, 0);
		Error *err = BuildError(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// ソケット監視設定
	FD_ZERO(&sock->_FDState);
	FD_SET(sock->_Socket, &sock->_FDState);

	return (SocketE){ NewError(false), sock };
}

extern SocketE NewUDPServerSocket(const in_port_t listenPort) {
	SocketE sockE = NewSocket(-1);
		if (!sockE.E->IsError(sockE.E)) sockE.E->Delete(sockE.E); else return (SocketE){ sockE.E };
	Socket *sock = sockE.Socket;

	// ソケット作成
	sock->_Socket = socket(PF_INET, SOCK_DGRAM, 0);
		Error *err = BuildError(sock->_Socket == -1, "ソケットの作成に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// 接続情報構成
	sock->_Addr = (struct sockaddr_in *)(malloc(sizeof(struct sockaddr_in)));
	memset(sock->_Addr, 0, sizeof(*sock->_Addr));
	sock->_Addr->sin_family			= AF_INET;
	sock->_Addr->sin_port			= htons(listenPort);
	sock->_Addr->sin_addr.s_addr	= htonl(INADDR_ANY);

	// 名前を付与
	int result = bind(sock->_Socket, (struct sockaddr *)(sock->_Addr), sizeof(*sock->_Addr));
		err = BuildError(result == -1, "名前付与に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (SocketE){ err };

	// ソケット監視設定
	FD_ZERO(&sock->_FDState);
	FD_SET(sock->_Socket, &sock->_FDState);

	return (SocketE){ NewError(false), sock };
}
