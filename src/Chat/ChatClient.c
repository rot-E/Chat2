#include "ChatClient.h"

static E Join(ChatClient_t *self, String_t *handle) {
	/* ハンドル名検査 */
	if (handle->GetLength(handle) > CHAT.HANDLE_LENGTH_MAX) return Error.Build(true, "ハンドル名の長さが不正");

	/* ハンドル名の設定 */
	self->_Handle = String.New(handle->Unpack(handle));

	/* 接続 */
	Socket_t_E sE = Socket.NewTCPClient(self->_Host, self->_Port);
		if (sE.E->IsError(sE.E)) { return sE.E; } else { Error.Delete(sE.E); }
	self->_Socket = sE.V;

	/* 参加 */
	// "JOIN {handle}" を送信
	String_t *str = String.NewFormat("JOIN %s", handle);
	Error_t *err = self->_Socket->Send(self->_Socket, str);
		if (!err->IsError(err)) Error.Delete(err); else return err;

	return Error.New(false);
}

static String_t *GetHandle(ChatClient_t *self) {
	return self->_Handle;
}

static E Post(ChatClient_t *self, String_t *message) {
	/* 投稿 */
	// "POST {message}" を送信
	String_t *str = String.NewFormat("POST %s", message->Unpack(message));
	Error_t *err = self->_Socket->Send(self->_Socket, str);
		if (!err->IsError(err)) Error.Delete(err); else return err;

	return Error.New(false);
}

static bool UpdateExists(ChatClient_t *self) {
	return self->_Socket->UpdateExists(self->_Socket);
}

static String_t_E GetMessage(ChatClient_t *self) {
	if (!self->UpdateExists(self)) return (String_t_E){ Error.Build(true, "更新無し") };

	/* 新規メッセージ受信 */
	String_t_E sE = self->_Socket->Receive(self->_Socket);
		if (!sE.E->IsError(sE.E)) Error.Delete(sE.E); else return (String_t_E){ sE.E };
	String_t *str = sE.V;

	// 解析
	String_t *mesgOutset = String.New("MESG ");
	bool_E bE = str->StartsWith(str, mesgOutset);
		if (!bE.E->IsError(bE.E)) Error.Delete(bE.E); else return (String_t_E){ bE.E };
		Error_t *err = Error.Build(!bE.V, "|MESG {message}| 形式外の文字列を受信");

	uint8_t_ptr_E ui8ptrE = str->Substring(str, mesgOutset->GetLength(mesgOutset), str->GetLength(str) + 1);
		if (!ui8ptrE.E->IsError(ui8ptrE.E)) Error.Delete(ui8ptrE.E); else return (String_t_E){ ui8ptrE.E };

	return (String_t_E){ Error.New(false), String.New(ui8ptrE.V) };
}

static E Quit(ChatClient_t *self) {
	/* 退出 */
	// "QUIT" を送信
	Error_t *err = self->_Socket->Send(self->_Socket, String.New("QUIT"));	
		if (!err->IsError(err)) Error.Delete(err); else return err;

	// 切断
	self->_Socket->Disconnect(self->_Socket);

	return Error.New(false);
}


static ChatClient_t *New(String_t *host, const in_port_t port) {
	ChatClient_t *cli = (ChatClient_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.NewADT(sizeof(ChatClient_t)));

	cli->Join			= Join;
	cli->GetHandle		= GetHandle;
	cli->Post			= Post;
	cli->UpdateExists	= UpdateExists;
	cli->GetMessage		= GetMessage;
	cli->Quit			= Quit;

	cli->_Host			= String.New(host->Unpack(host));
	cli->_Port			= port;
	cli->_Socket		= NULL;
	cli->_Handle		= NULL;

	return cli;
}

static void Release(ChatClient_t *cli) {
	String.Delete(cli->_Host);
	Socket.Delete(cli->_Socket);
	String.Delete(cli->_Handle);
}

static void Delete(ChatClient_t *cli) {
	ChatClient.Release(cli);
	free(cli);
}


_ChatClient ChatClient = {
	.New		= New,
	.Release	= Release,
	.Delete		= Delete,
};
