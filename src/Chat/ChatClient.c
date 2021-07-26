#include "ChatClient.h"

method static void Join(ChatClient_t *cli, String_t *handle) throws (Chat.HandleException, Chat.NetworkException) {
	/* ハンドル名検査 */
	if (String.GetLength(handle) > Chat.HANDLE_LENGTH_MAX) {
		String_t *msg = String.NewFormat("ハンドル名が%d文字を超過", Chat.HANDLE_LENGTH_MAX);
		defer {
			String.Delete(msg);
		} set execute {
			throw (Signal.Build(Chat.HandleException, String.Unpack(msg)));
		} ret
	}

	/* ハンドル名の設定 */
	cli->_Handle = String.New(String.Unpack(handle));

	/* 接続 */
	try {
		cli->_Socket = Socket.NewTCPClient(cli->_Host, cli->_Port);
	} catch (Socket.Exception) {
		String.Delete(cli->_Handle);
		throw (Signal.Build(Chat.NetworkException, "接続失敗"));
	} fin

	/* 参加 */
	// "JOIN {handle}" を送信
	String_t *str = String.NewFormat("JOIN %s", String.Unpack(handle));
	try {
		Socket.Send(cli->_Socket, str);
	} catch (Socket.Exception) {
		throw (Signal.Build(Chat.NetworkException, "送信失敗/JOIN"));
	} finally {
		String.Delete(str);
	} end
}

method static String_t *GetHandle(ChatClient_t *cli) {
	return cli->_Handle;
}

method static void Post(ChatClient_t *cli, String_t *message) throws (Chat.NetworkException) {
	/* 投稿 */
	// "POST {message}" を送信
	String_t *str = String.NewFormat("POST %s", String.Unpack(message));
	try {
		Socket.Send(cli->_Socket, str);
	} catch (Socket.Exception) {
		throw (Signal.Build(Chat.NetworkException, "送信失敗/POST"));
	} finally {
			String.Delete(str);
	} end
}

method static bool UpdateExists(ChatClient_t *cli) {
	return Socket.UpdateExists(cli->_Socket);
}

method static String_t *GetMessage(ChatClient_t *cli) throws (Chat.Exception, Chat.NetworkException) {
	if (!ChatClient.UpdateExists(cli))
		throw (Signal.Build(Chat.Exception, "更新無し"));

	/* 新規メッセージ受信 */
	String_t *str = NULL;
	try {
		str = Socket.Receive(cli->_Socket);
	} catch (Socket.DisconnectionException) {
		throw (Signal.Build(Chat.NetworkException, "コネクション切断"));
	} catchN (Socket.Exception) {
		throw (Signal.Build(Chat.NetworkException, "受信失敗"));
	} fin

	// 解析
	String_t *mesgOutset = String.New("MESG ");
	if (!String.StartsWith(str, mesgOutset))
		throw (Signal.Build(Chat.Exception, "|MESG {message}| 形式外の文字列を受信"));

	defer {
		String.Delete(str);
		String.Delete(mesgOutset);
	} set retrieve {
		return String.Substring(str, String.GetLength(mesgOutset), String.GetLength(str) + 1);
	} ret
}

method static void Quit(ChatClient_t *cli) throws (Chat.NetworkException) {
	/* 退出 */
	// "QUIT" を送信
	try {
		Socket.Send(cli->_Socket, String.New("QUIT"));
	} catch (Socket.Exception) {
		throw (Signal.Build(Chat.NetworkException, "送信失敗/QUIT"));
	} fin

	// 切断
	Socket.Disconnect(cli->_Socket);
}

method static ChatClient_t *New(String_t *host, const in_port_t port) {
	ChatClient_t *cli = (ChatClient_t *)(_Memory.Allocate(sizeof(ChatClient_t)));

	cli->_Host			= String.New(String.Unpack(host));
	cli->_Port			= port;
	cli->_Socket		= NULL;
	cli->_Handle		= NULL;

	cli->Join			= Join;
	cli->GetHandle		= GetHandle;
	cli->Post			= Post;
	cli->UpdateExists	= UpdateExists;
	cli->GetMessage		= GetMessage;
	cli->Quit			= Quit;

	return cli;
}

method static void Delete(ChatClient_t *cli) {
	String.Delete(cli->_Host);
	Socket.Delete(cli->_Socket);
	String.Delete(cli->_Handle);
	free(cli);
}

_ChatClient ChatClient = {
	.New			= New,
	.Delete			= Delete,

	.Join			= Join,
	.GetHandle		= GetHandle,
	.Post			= Post,
	.UpdateExists	= UpdateExists,
	.GetMessage		= GetMessage,
	.Quit			= Quit,
};
