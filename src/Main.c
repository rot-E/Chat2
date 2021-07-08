#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "SPEC/ESCAPE_SEQUENCE.h"
#include "Error/Error.h"
#include "String/String.h"
#include "Console/Console.h"
#include "Socket/Socket.h"

#include "Chat/CHAT.h"
#include "Chat/ChatClient.h"
#include "Chat/CHAT.h"

#include "EControl.h"

typedef enum {
	Client, Server
} Mode;

typedef struct {
	Mode m;
	String_t *ip;
} Result;

Result ping(const bool v, const int timeout, const in_port_t port) {
	if (v)
		Console.WriteLine(String.NewFormat(
			"%s[情報] ポート番号 %s%d%s でチャットサーバの存在を確認中...%s",
				ESCAPE_SEQUENCE.TEXT.YELLOW,
				ESCAPE_SEQUENCE.TEXT.CYAN,
				port,
				ESCAPE_SEQUENCE.TEXT.YELLOW,
				ESCAPE_SEQUENCE.TEXT.DEFAULT
		));

	Socket_t *pinger = UnwrapE(Socket.NewUDPClient());
	InspectE(pinger->ConfigureBroadcast(pinger, port));

	struct sigaction action = {
		.sa_handler = ({
			void handler(int signo) {
				return;
			};
			handler;
		}),
		.sa_flags = 0,
	};
	InspectE(
		Error.Build(
			sigfillset(&action.sa_mask) == -1 ||
			sigaction(SIGALRM, &action, NULL) == -1,
			"シグナルハンドラの設定に失敗"
		)
	);

	for (int i = 0; i < 3; i++) {
		InspectE(pinger->Broadcast(pinger, String.New("HELO")));
		if (v)
			Console.WriteLine(String.NewFormat(
				"%s[情報] HELOメッセージをブロードキャスト%s",
				ESCAPE_SEQUENCE.TEXT.YELLOW,
				ESCAPE_SEQUENCE.TEXT.DEFAULT
			));

		alarm(timeout);
		String_t_E sE = pinger->Collect(pinger);
		if (sE.E->IsError(sE.E)) {
			if (errno == EINTR) {
				// タイムアウト
				if (v) {
					Console.SetColour(String.New(ESCAPE_SEQUENCE.TEXT.YELLOW));
					Console.WriteLine(String.NewFormat("[情報] 応答なし (%d秒間)", timeout));
					Console.WriteLine(String.New("[情報] 再送"));
					Console.SetDefaultColour();
				}
				continue;
			} else {
				InspectE(sE.E);
			}
		}
		alarm(0);

		if (sE.V->Equals(sE.V, String.New("HERE"))) {
			if (v)
				Console.WriteLine(String.NewFormat(
					"%s[情報] HEREメッセージを %s%s%s より受信%s",
					ESCAPE_SEQUENCE.TEXT.YELLOW,
					ESCAPE_SEQUENCE.TEXT.CYAN,
					({
						String_t *ip = pinger->GetDestIPAddr(pinger);
						ip->Unpack(ip);
					}),
					ESCAPE_SEQUENCE.TEXT.YELLOW,
					ESCAPE_SEQUENCE.TEXT.DEFAULT
				));

			return (Result){ Client, pinger->GetDestIPAddr(pinger) };
		}
	}

	return (Result){ Server, NULL };
}

void svrSh(const bool v, const in_port_t port) {
	Error_t *err = Error.Build(true, "サーバが見つかりませんでした");
	err->ExitIfError(err);

	if (v)
		Console.WriteLine(String.NewFormat(
			"%s[情報] チャットサーバとして動作中...%s",
			ESCAPE_SEQUENCE.TEXT.YELLOW,
			ESCAPE_SEQUENCE.TEXT.DEFAULT
		));
}

void cliSh(const bool v, String_t *ip, const in_port_t port, String_t *handle) {
	if (v)
		Console.WriteLine(String.NewFormat(
			"%s[情報] チャットクライアントとして動作中...%s",
			ESCAPE_SEQUENCE.TEXT.YELLOW,
			ESCAPE_SEQUENCE.TEXT.DEFAULT
		));

	ChatClient_t *cli = ChatClient.New(ip, port);
	InspectE(cli->Join(cli, handle));
	Console.WriteLine(String.NewFormat(
		"%s%s[ハンドル名 %s%s%s として参加]%s",
		(v)? "\n" : "",
		ESCAPE_SEQUENCE.TEXT.GREEN,
		ESCAPE_SEQUENCE.TEXT.CYAN,
		handle->Unpack(handle),
		ESCAPE_SEQUENCE.TEXT.GREEN,
		ESCAPE_SEQUENCE.TEXT.DEFAULT
	));

	if (v)
		Console.WriteLine(String.NewFormat(
			"%s[情報] 終了するには QUIT と打鍵してください%s",
			ESCAPE_SEQUENCE.TEXT.YELLOW,
			ESCAPE_SEQUENCE.TEXT.DEFAULT
		));
	Console.WriteLine(String.New("------------------------------------------------------------"));

	fd_set state;
	FD_ZERO(&state);
	FD_SET(0, &state);

	// シェル動作
	bool prompt = true;
	for (;;) {
		if (prompt) {
			Console.Write(String.New("> "));
			prompt = false;
		}

		fd_set tmp = state;
		select(1, &tmp, NULL, NULL, &(struct timeval){ .tv_sec = 0, .tv_usec = 0 });

		/* 標準入力がある場合 */
		if (FD_ISSET(0, &tmp)) {
			String_t *str = String.NewN(Socket.DATA_MAX_SIZE);
			fgets(str->Unpack(str), Socket.DATA_MAX_SIZE, stdin);
			String_t *str2 = String.New(UnwrapE(str->DropLastChar(str)));
			String.Delete(str);

			if (str2->GetLength(str2) == 0) {
				// do nothing
			} else if (str2->Equals(str2, String.New("QUIT"))) {
				/* 退出 */
				InspectE(cli->Quit(cli));
				exit(EXIT_SUCCESS);
			} else {
				InspectE(cli->Post(cli, str2));

				Console.WriteColourLine(
					String.New(ESCAPE_SEQUENCE.TEXT.RED),
					String.NewFormat(
						"[%s] %s",
						handle->Unpack(handle),
						str2->Unpack(str2)
					)
				);
			}

			prompt = true;
		}

		/* 更新がある場合 */
		if (cli->UpdateExists(cli)) {
			Console.CarriageReturn();
			Console.WriteColourLine(
				String.New(ESCAPE_SEQUENCE.TEXT.BLUE),
				UnwrapE(cli->GetMessage(cli))
			);

			prompt = true;
		}
	}

	InspectE(cli->Quit(cli));
	ChatClient.Delete(cli);	
}

extern char *optarg;
extern int optind, opterr, optopt;

void main(const int argc, char* argv[]) {
	/* デフォルト値 ----------------------------------*/
	in_port_t port		= 50001;
	int32_t timeout		= 1;
	bool v				= false;
	/* --------------------------------------------*/

	/* コマンドライン引数の処理 */
	String_t *handle = String.New("");

	for (;;) {
		int c = getopt(argc, argv, "u:p:t:vh");
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
				Console.WriteErrorLine(String.NewFormat("使用法： %s -u [Handle] (-p [Port]) (-t [Timeout]) (-v)", argv[0]));
				exit(EXIT_SUCCESS);
		}
	}
	InspectE(Error.Build(UnwrapE(handle->GetCharAt(handle, 0)) == '\0', "ハンドル名不指定"));

	/* C/S決定 */
	Result r = ping(v, timeout, port);

	/* チャット */
	if (r.m == Server) {
		svrSh(v, port); /* Server動作 */
	} else {
		cliSh(v, r.ip, port, handle); /* Client動作 */
	}
}
