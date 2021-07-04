#include "ChatClient.h"

static E Join(ChatClient *self, const char *handle) {
	/* ハンドル名検査 */
	if (strlen(handle) > HANDLE_LENGTH_MAX) return BuildError(true, "ハンドル名の長さが不正");

	/* ハンドル名の設定 */
	self->_Handle = (char *)(malloc(sizeof(char) * (strlen(handle) + 1)));
		Error *err = BuildError(self->_Handle == NULL, "動的メモリ確保に失敗");
		if (!err->IsError(err)) err->Delete(err); else return err;
	strcpy(self->_Handle, handle);

	/* 接続 */
	SocketE sE = NewTCPClientSocket(self->_Host, self->_Port);
		if (sE.E->IsError(sE.E)) { return sE.E; } else { sE.E->Delete(sE.E); }
	self->_Socket = sE.Socket;

	/* 参加 */
	// "JOIN {handle}" を送信
	char str[DATA_MAX_SIZE];
	sprintf(str, "JOIN %s", handle);
	err = self->_Socket->Send(self->_Socket, str);
		if (!err->IsError(err)) err->Delete(err); else return err;

	return NewError(false);
}

static char *GetHandle(ChatClient *self) {
	return self->_Handle;
}

static E Post(ChatClient *self, const char *message) {
	/* 投稿 */
	// "POST {message}" を送信
	char str[DATA_MAX_SIZE];
	sprintf(str, "POST %s", message);
	Error *err = self->_Socket->Send(self->_Socket, str);
		if (!err->IsError(err)) err->Delete(err); else return err;

	return NewError(false);
}

static bool UpdateExists(ChatClient *self) {
	return self->_Socket->UpdateExists(self->_Socket);
}

static StringE GetMessage(ChatClient *self) {
	if (!self->UpdateExists(self)) return (StringE){ BuildError(true, "更新はありません") };

	/* 新規メッセージ受信 */
	StringE sE = self->_Socket->Receive(self->_Socket);
		if (!sE.E->IsError(sE.E)) sE.E->Delete(sE.E); else return (StringE){ sE.E };
	char *str = sE.String;

	// 解析
	str = strstr(str, "MESG ");
		Error *err = BuildError(str == NULL, "MESG {message} の形式ではない文字列を受信");

	return (StringE){ NewError(false), str + strlen("MESG ") };
}

static E Quit(ChatClient * self) {
	/* 退出 */
	// "QUIT" を送信
	Error *err = self->_Socket->Send(self->_Socket, "QUIT");	
		if (!err->IsError(err)) err->Delete(err); else return err;

	// 切断
	self->_Socket->Disconnect(self->_Socket);

	return NewError(false);
}

static void Delete(ChatClient *self) {
	free(self->_Handle);
	free(self->_Host);
	free(self);
}

extern ChatClientE NewChatClient(const char *host, const int port) {
	ChatClient *cli = (ChatClient *)(malloc(sizeof(ChatClient)));
		Error *err = BuildError(cli == NULL, "動的メモリ確保に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (ChatClientE){ err };

	cli->Join			= Join;
	cli->GetHandle		= GetHandle;
	cli->Post			= Post;
	cli->UpdateExists	= UpdateExists;
	cli->GetMessage		= GetMessage;
	cli->Quit			= Quit;

	cli->Delete			= Delete;

	cli->_Host			= (char *)(malloc(sizeof(char) * (strlen(host) + 1)));
		err = BuildError(cli->_Host == NULL, "動的メモリ確保に失敗");
		if (!err->IsError(err)) err->Delete(err); else return (ChatClientE){ err };
	strcpy(cli->_Host, host);

	cli->_Port			= port;
	cli->_Socket		= NULL;
	cli->_Handle		= NULL;

	return (ChatClientE){ NewError(false), cli };
}
