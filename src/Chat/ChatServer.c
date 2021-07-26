#include "ChatServer.h"

static bool _SocketComparator(any *mapK, any *k) {
	return ((Socket_t *)(mapK))->_Socket == ((Socket_t *)(k))->_Socket;
}

static bool _StringComparator(any *mapV, any *v) {
	return String.Equals(mapV, v);
}

/* 応答 ---------------------------------------------- */
static int _responde(any *arg) {
	ChatServer_t *svr = (ChatServer_t *)(arg);

	String_t *helo_s = String.New("HELO");
	String_t *here_s = String.New("HERE");

	for (;;) { // TODO: 例外機構をスレッドセーフにしてエラー処理実装
		Socket_t *sock = Socket.NewUDPServer(svr->_Port);
		for (;;) {
			if (Socket.UpdateExists(sock)) {
				String_t *req = Socket.Collect(sock);
				if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
					"[情報] 受信: %s",
					String.Unpack(req)
				));

				if (String.Equals(req, helo_s)) {
					if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
						"[情報] 受信/HELO: %s:%d",
						String.Unpack(Socket.GetDestIPAddr(sock)),
						Socket.GetDestPort(sock)
					));

					Socket.Cast(sock, here_s);

					if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
						"[情報] 送信/HERE: %s:%d",
						String.Unpack(Socket.GetDestIPAddr(sock)),
						Socket.GetDestPort(sock)
					));
				}
				String.Delete(req);
			}
		}
	}
}
/* -------------------------------------------------- */

/* 応接 ---------------------------------------------- */
static int _recept(any *arg) {
	ChatServer_t *svr = (ChatServer_t *)(arg);

	for (;;) { // TODO: 例外機構をスレッドセーフにしてエラー処理実装
		Socket_t *sock = Socket.Accept(svr->_Socket);

		/* "JOIN {handle}" を受信 */
		// TODO: alarm→タイムアウト=>Receive中断=>continue;
		String_t *req = Socket.Receive(sock);

		if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
			"[情報] 受信: %s",
			String.Unpack(req)
		));

		// {handle}切り出し
		String_t *handle = String.Substring(req, String.GetLength(String.New("JOIN ")), String.GetLength(req) + 1);

		if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] 参加/%s", String.Unpack(handle)));

		/* 存在管理 */
		Map.Put(svr->_User, sock, handle);
		FD_SET(sock->_Socket, &svr->_State);
		svr->_Fd_Max = (sock->_Socket > svr->_Fd_Max)? sock->_Socket : svr->_Fd_Max;
	}
}
/* -------------------------------------------------- */

/* チャット ------------------------------------------- */
static int _chat(any *arg) {
	ChatServer_t *svr = (ChatServer_t *)(arg);

	for (;;) { // TODO: 例外機構をスレッドセーフにしてエラー処理実装
		for (int32_t i = 0; i < Map.GetLength(svr->_User); i++) { //
			Item_t user = Map.Get(svr->_User, i);

			if (Socket.UpdateExists(user.Key)) {
				String_t *req = NULL;
				bool disconn = false;
				try { // ここに於ては例外機構を使わざるを得ない→他スレッド(含main)では使えない
					req = Socket.Receive(user.Key);
				} catch (Socket.DisconnectionException) {
					disconn = true;
				} fin

				if (disconn) {
					/* コネクション切断 */
					Socket.Disconnect(user.Key);
					if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] 切断/%s", String.Unpack(user.Value)));

					/* 存在抹消 */
					Map.Remove(svr->_User, user.Key);

					FD_CLR(Socket.GetFd(user.Key), &svr->_State);
					if (svr->_Fd_Max == Socket.GetFd(user.Key)) {
						int32_t max = 0;
						for (int32_t j = 0; j < Map.GetLength(svr->_User); j++)
							max = (max > Socket.GetFd(Map.Get(svr->_User, j).Key))?
								max : Socket.GetFd(Map.Get(svr->_User, j).Key);
					}

					Socket.Delete(user.Key);
					String.Delete(user.Value);

					String.Delete(req);
					break;
				}

				if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
					"[情報] 受信: %s",
					String.Unpack(req)
				));

				if (String.Equals(req, String.New("QUIT"))) {
					/* コネクション切断 */
					Socket.Disconnect(user.Key);
					if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] 退出/%s", String.Unpack(user.Value)));

					/* 存在抹消 */
					Map.Remove(svr->_User, user.Key);

					FD_CLR(Socket.GetFd(user.Key), &svr->_State);
					if (svr->_Fd_Max == Socket.GetFd(user.Key)) {
						int32_t max = 0;
						for (int32_t j = 0; j < Map.GetLength(svr->_User); j++)
							max = (max > Socket.GetFd(Map.Get(svr->_User, j).Key))?
								max : Socket.GetFd(Map.Get(svr->_User, j).Key);
					}

					Socket.Delete(user.Key);
					String.Delete(user.Value);

					String.Delete(req);
					break;

				} else if (String.StartsWith(req, String.New("POST "))) {
					String_t *msg = String.Substring(req, String.GetLength(String.New("POST ")), String.GetLength(req) + 1);
					if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] 投稿/[%s] %s", String.Unpack(user.Value), String.Unpack(msg)));

					for (int32_t j = 0; j < Map.GetLength(svr->_User); j++) { //
						if (j == i) continue;
						Socket.Send(Map.Get(svr->_User, j).Key, String.NewFormat("MESG [%s] %s", String.Unpack(user.Value), String.Unpack(msg)));
					}

					if (svr->_V) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] 全体送信/[%s] %s", String.Unpack(user.Value), String.Unpack(msg)));

					Console.WriteChar(CC.BEL);
					Console.WriteColourLine(
						SGR.TEXT.BLUE,
						String.NewFormat("[%s] %s", String.Unpack(user.Value), String.Unpack(msg))
					);
					String.Delete(req);
					break;
				}

				String.Delete(req);
			}
		}
	}
}
/* -------------------------------------------------- */

/* svr->_Users表示 ----------------------------------- */
static int _print_users(any *arg) {
	ChatServer_t *svr = (ChatServer_t *)(arg);

	for (;;) {
		System.Sleep(3);
		Console.WriteLine(String.New("--------------------"));
		for (int32_t i = 0; i < Map.GetLength(svr->_User); i++) {
			Console.WriteLine(String.NewFormat(
				"%d:%s",
				Socket.GetFd(Map.Get(svr->_User, i).Key),
				String.Unpack(Map.Get(svr->_User, i).Value)
			));
		}
		Console.WriteLine(String.New("--------------------"));
	}
}
/* -------------------------------------------------- */

method static void StartResponder(ChatServer_t *svr) throws (Chat.Exception) {
	thrd_t thr_responde;

	int result = thrd_create(&thr_responde, _responde, svr);
	if (result != thrd_success)
		throw (Signal.Build(Chat.Exception, "スレッドの作成に失敗"));
}

method static void StartServer(ChatServer_t *svr) throws (Chat.Exception) {
	svr->_Socket = Socket.NewTCPServer(svr->_Port);

	thrd_t thr_recept;
	int result = thrd_create(&thr_recept, _recept, svr);
	if (result != thrd_success)
		throw (Signal.Build(Chat.Exception, "スレッドの作成に失敗"));

	thrd_t thr_chat;
	result = thrd_create(&thr_chat, _chat, svr);
	if (result != thrd_success)
		throw (Signal.Build(Chat.Exception, "スレッドの作成に失敗"));

	/*
	thrd_t thr_print_users;
	result = thrd_create(&thr_print_users, _print_users, svr);
	if (result != thrd_success)
		throw (Signal.Build(Chat.Exception, "スレッドの作成に失敗"));
	*/
}

method static void Post(ChatServer_t *svr, String_t *message) throws (Chat.Exception) {
	for (int32_t i = 0; i < Map.GetLength(svr->_User); i++) {
		Socket.Send(Map.Get(svr->_User, i).Key, String.NewFormat("MESG [%s] %s", String.Unpack(svr->_Handle), String.Unpack(message)));
	}
}

method static ChatServer_t *New(String_t *handle, const in_port_t port, const bool v) {
	ChatServer_t *svr = (ChatServer_t *)(_Memory.Allocate(sizeof(ChatServer_t)));

	svr->_Handle			= String.New(String.Unpack(handle));
	svr->_Port				= port;
	svr->_Socket			= NULL;

	svr->_User				= Map.New() in (Socket_t *, String_t *);
	svr->_User->SetComparator(svr->_User, _SocketComparator, _StringComparator);
	FD_ZERO(&svr->_State);
	svr->_Fd_Max			= 0;

	svr->_V					= v;

	svr->StartResponder		= StartResponder;
	svr->StartServer		= StartServer;
	svr->Post				= Post;

	return svr;
}

method static void Delete(ChatServer_t *svr) {
	Socket.Disconnect(svr->_Socket);
	Socket.Delete(svr->_Socket);
	free(svr);
}

_ChatServer ChatServer = {
	.New				= New,	
	.Delete				= Delete,

	.StartResponder		= StartResponder,
	.StartServer		= StartServer,
	.Post				= Post,
};
