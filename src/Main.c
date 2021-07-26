#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <NeoC/NeoC.h>

#include "Chat/Chat.h"
#include "Chat/ChatClient.h"
#include "Chat/ChatServer.h"

SignalCode_t MainException;

typedef enum {
	Client, Server
} Mode;

typedef struct {
	Mode m;
	String_t *ip;
} Result;

Result ping(const bool v, const int32_t timeout, const in_port_t port) throws (MainException) {
	if (v)
		Console.WriteLine(String.NewFormat(
			"%s[情報] ポート番号 %s%d%s のチャットサーバの存在を確認中...%s",
				SGR.TEXT.YELLOW,
				SGR.TEXT.CYAN,
				port,
				SGR.TEXT.YELLOW,
				SGR.TEXT.DEFAULT
		));

	Socket_t *pinger = NULL;
	try {
		pinger = Socket.NewUDPClient();
		Socket.ConfigureBroadcast(pinger, port);
	} catch (Socket.Exception) {
		throw (Signal.Build(MainException, "通信失敗"));
	} fin

	struct sigaction action = {
		.sa_handler = ({
			void handler(int signo) {
				return;
			};
			handler;
		}),
		.sa_flags = 0,
	};

	if (sigfillset(&action.sa_mask) == -1 || sigaction(SIGALRM, &action, NULL) == -1)
			throw (Signal.Build(MainException, "シグナルハンドラの設定に失敗"));

	for (int i = 0; i < 3; i++) {
		bool cont = false;
		try {
			Socket.Broadcast(pinger, String.New("HELO"));
		} catch (Socket.Exception) {
			throw (Signal.Build(MainException, "送信失敗/HELO"));
		} fin

		if (v)
			Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
				"[情報] HELOメッセージをブロードキャスト"
			));

		alarm(timeout);
		String_t *str = NULL;
		try {
			str = Socket.Collect(pinger);
		} catch (Socket.Exception) {
			if (errno == EINTR) {
				// タイムアウト
				if (v) {
					Console.SetColour(SGR.TEXT.YELLOW);
					Console.WriteLine(String.NewFormat("[情報] 応答なし (%d秒間)", timeout));
					Console.WriteLine(String.New("[情報] 再送"));
					Console.SetDefaultColour();
				}
				cont = true;
			} else {
				throw (Signal.Build(MainException, "受信失敗"));
			}
		} fin
		alarm(0);
		if (cont) continue;

		if (String.Equals(str, String.New("HERE"))) {
			if (v)
				Console.WriteLine(String.NewFormat(
					"%s[情報] HEREメッセージを %s%s:%d%s より受信%s",
					SGR.TEXT.YELLOW,
					SGR.TEXT.CYAN,
					({
						String_t *ip = pinger->GetDestIPAddr(pinger);
						String.Unpack(ip);
					}),
					pinger->GetDestPort(pinger),
					SGR.TEXT.YELLOW,
					SGR.TEXT.DEFAULT
				));

			return (Result){ Client, Socket.GetDestIPAddr(pinger) };
		}
	}

	return (Result){ Server, NULL };
}

void svrSh(const bool v, const in_port_t port, String_t *handle) throws (MainException) {
	if (v) Console.WriteColourLine(SGR.TEXT.YELLOW, String.New(u8"[情報] チャットサーバとして動作中..."));

	ChatServer_t *svr = ChatServer.New(handle, port, v);
	svr->StartResponder(svr);
	if (v) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] %d番UDPポートでHELOメッセージ応答中", port));

	svr->StartServer(svr);
	if (v) Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat("[情報] %d番TCPポートで待受中", port));

	Console.WriteLine(String.NewFormat(
		"%s[ハンドル名 %s%s%s として参加]%s",
		SGR.TEXT.GREEN,
		SGR.TEXT.CYAN,
		handle->Unpack(handle),
		SGR.TEXT.GREEN,
		SGR.TEXT.DEFAULT
	));

	// シェル動作
	for (;;) {
		/* 標準入力がある場合 */
		if (System.Keystroked()) {
			String_t *str = String.NewN(Socket.DATA_MAX_SIZE);
			fgets(str->Unpack(str), Socket.DATA_MAX_SIZE, stdin);
			String_t *str2 = str->DropLastChar(str);
			String.Delete(str);

			Console.ErasePrevLine();

			if (str2->GetLength(str2) == 0) {
				Console.WriteChar(CC.BEL);

				// do nothing
			} else {
				svr->Post(svr, str2);

				String_t *line = String.NewFormat(
					"[%s] %s",
					String.Unpack(handle),
					String.Unpack(str2)
				);
				Console.WriteColourLine(SGR.TEXT.RED, line);
				String.Delete(line);
			}

			String.Delete(str2);
		}
	}
}

void cliSh(const bool v, String_t *ip, const in_port_t port, String_t *handle) throws (Chat.HandleMainException, Chat.NetworkMainException, Chat.MainException) {
	if (v)
		Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
			"[情報] チャットクライアントとして動作中..."
		));

	ChatClient_t *cli = ChatClient.New(ip, port);
	cli->Join(cli, handle);

	Console.WriteLine(String.NewFormat(
		"%s%s[ハンドル名 %s%s%s として参加]%s",
		(v)? "\n" : "",
		SGR.TEXT.GREEN,
		SGR.TEXT.CYAN,
		handle->Unpack(handle),
		SGR.TEXT.GREEN,
		SGR.TEXT.DEFAULT
	));

	if (v)
		Console.WriteColourLine(SGR.TEXT.YELLOW, String.NewFormat(
			"[情報] 終了するには QUIT と打鍵してください"
		));
	Console.WriteLine(String.New("------------------------------------------------------------"));

	// シェル動作
	bool prompt = true;
	String_t *pt = String.New("> ");
	String_t *quit_s = String.New("QUIT");
	for (;;) {
		if (prompt) {
			Console.Write(pt);
			prompt = false;
		}

		/* 標準入力がある場合 */
		if (System.Keystroked()) {
			String_t *str = String.NewN(Socket.DATA_MAX_SIZE);
			fgets(str->Unpack(str), Socket.DATA_MAX_SIZE, stdin);
			String_t *str2 = str->DropLastChar(str);
			String.Delete(str);

			Console.ErasePrevLine();

			if (str2->GetLength(str2) == 0) {
				Console.WriteChar(CC.BEL);

				// do nothing
			} else if (str2->Equals(str2, quit_s)) {
				/* 退出 */
				cli->Quit(cli);
				exit(EXIT_SUCCESS);
			} else {
				cli->Post(cli, str2);

				String_t *line = String.NewFormat(
					"[%s] %s",
					String.Unpack(handle),
					String.Unpack(str2)
				);
				Console.WriteColourLine(SGR.TEXT.RED, line);
				String.Delete(line);
			}

			String.Delete(str2);
			prompt = true;
		}

		/* 更新がある場合 */
		if (cli->UpdateExists(cli)) {
			Console.WriteChar(CC.BEL);

			Console.WriteChar(CC.CR);
			Console.WriteColourLine(
				SGR.TEXT.BLUE,
				cli->GetMessage(cli)
			);

			prompt = true;
		}
	}

	cli->Quit(cli);
	ChatClient.Delete(cli);	
}

extern char *optarg;
extern int32_t optind, opterr, optopt;

void main(const int32_t argc, uint8_t *argv[]) $_ {
	/* デフォルト値 ----------------------------------*/
	in_port_t port		= 50001;
	int32_t timeout		= 1;
	bool v				= false;
	/* --------------------------------------------*/

	/* コマンドライン引数の処理 */
	String_t *handle = String.New("");

	for (;;) {
		int c = getopt(argc, (char **)(argv), "u:p:t:vh");
		if (c == -1) break;

		opterr = 0;
		switch(c) {
			case 'u':
				String.Delete(handle);
				handle = String.New(optarg);
				break;

			case 't':
				timeout = atoi(optarg);
				break;

			case 'p':
				port = atoi(optarg);
				break;

			case 'v':
				v = true;
				break;

			case 'h':
				Console.WriteErrorLine(String.NewFormat("使用法： %s -u [Handle] (-p [Server Port]) (-t [Timeout]) (-v)", argv[0]));
				exit(EXIT_SUCCESS);
		}
	}

	MainException signal;

	if (String.GetCharAt(handle, 0) == '\0')
		throw (Signal.Build(MainException, "ハンドル名不指定"));

	/* C/S決定 */
	Result r = ping(v, timeout, port);

	/* チャット */
	Chat.Setup();
	if (r.m == Server) {
		/* Server動作 */
		svrSh(v, port, handle);
	} else {
		/* Client動作 */
		cliSh(v, r.ip, port, handle);
	}
} _$
