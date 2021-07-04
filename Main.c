#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "Error/Error.h"
#include "Socket/Socket.h"
#include "Chat/ChatClient.h"
#include "Chat/ChatData.h"

#include "EControl.h"

typedef enum {
	Client, Server
} Mode;

typedef struct {
	Mode m;
	char *ip;
} Result;

Result ping(const bool v, const int timeout, const in_port_t port) {
	if (v) printf("%s[情報] ポート番号 %s%d%s でチャットサーバの存在を確認中...%s\n", "\x1b[33m", "\x1b[36m", port, "\x1b[33m", "\x1b[39m");

	Socket *pinger = UnwrapE(NewUDPClientSocket());
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
		BuildError(
			sigfillset(&action.sa_mask) == -1 ||
			sigaction(SIGALRM, &action, NULL) == -1,
			"シグナルハンドラの設定に失敗"
		)
	);

	for (int i = 0; i < 3; i++) {
		InspectE(pinger->Broadcast(pinger, "HELO"));
		if (v) printf("%s[情報] HELOメッセージをブロードキャスト%s\n", "\x1b[33m", "\x1b[39m");

		alarm(timeout);
		StringE sE = pinger->Collect(pinger);
		if (sE.E->IsError(sE.E)) {
			if (errno == EINTR) {
				// タイムアウト
				if (v) printf("%s[情報] 応答なし (%d秒間)%s\n", "\x1b[33m", timeout, "\x1b[39m");
				if (v) printf("%s[情報] 再送%s\n", "\x1b[33m", "\x1b[39m");
				continue;
			} else {
				InspectE(sE.E);
			}
		}
		alarm(0);

		if (!strcmp(sE.String, "HERE")) {
			if (v) printf("%s[情報] HEREメッセージを受信%s\n", "\x1b[33m", "\x1b[39m");

			return (Result){ Client, pinger->GetDestIPAddr(pinger) };
		}
	}

	return (Result){ Server, NULL };
}

void svrSh(const bool v, const in_port_t port) {
	Error *err = BuildError(true, "サーバが見つかりませんでした");
	err->ExitIfError(err);

	if (v) printf("%s[情報] チャットサーバとして動作中...%s\n", "\x1b[33m", "\x1b[39m");
}

void cliSh(const bool v, const char *ip, const in_port_t port, char *handle) {
	if (v) printf("%s[情報] チャットクライアントとして動作中...%s\n", "\x1b[33m", "\x1b[39m");

	ChatClient *cli = UnwrapE(NewChatClient(ip, port));
	InspectE(cli->Join(cli, handle));
	printf("%s%s[ハンドル名 %s%s%s として参加]%s\n", (v)? "\n" : "", "\x1b[32m", "\x1b[36m", handle, "\x1b[32m", "\x1b[39m");

	if (v) printf("%s[情報] 終了するには QUIT と打鍵してください%s\n", "\x1b[33m", "\x1b[39m");
	printf("------------------------------------------------------------\n");

	fd_set state;
	FD_ZERO(&state);
	FD_SET(0, &state);

	// シェル動作
	bool prompt = true;
	for (;;) {
		if (prompt) {
			printf("> "); fflush(stdout);
			prompt = false;
		}

		fd_set tmp = state;
		select(1, &tmp, NULL, NULL, &(struct timeval){ .tv_sec = 0, .tv_usec = 0 });

		/* 標準入力がある場合 */
		if (FD_ISSET(0, &tmp)) {
			char str[DATA_MAX_SIZE];
			fgets(str, DATA_MAX_SIZE, stdin);
			str[strlen(str) - 1] = '\0'; // 改行文字消去

			if (strlen(str) == 0) {
				// do nothing
			} else if (!strcmp(str, "QUIT")) {
				/* 退出 */
				InspectE(cli->Quit(cli));
				exit(EXIT_SUCCESS);
			} else {
				InspectE(cli->Post(cli, str));

				printf("%s[%s] %s%s\n", "\x1b[31m", cli->GetHandle(cli), str, "\x1b[39m");
				fflush(stdout);
			}

			prompt = true;
		}

		/* 更新がある場合 */
		if (cli->UpdateExists(cli)) {
			printf("\r");
			printf("%s%s%s\n", "\x1b[34m", UnwrapE(cli->GetMessage(cli)), "\x1b[39m"); fflush(stdout);
			fflush(stdout);

			prompt = true;
		}
	}

	InspectE(cli->Quit(cli));
	cli->Delete(cli);	
}

extern char *optarg;
extern int optind, opterr, optopt;

void main(const int argc, char* argv[]) {
	/* デフォルト値 ----------------------------------*/
	in_port_t port	= 50001;
	int timeout		= 1;
	bool v			= false;
	/* --------------------------------------------*/

	/* コマンドライン引数の処理 */
	char handle[HANDLE_LENGTH_MAX];
	handle[0] = '\0';

	for (;;) {
		int c = getopt(argc, argv, "u:p:t:vh");
		if (c == -1) break;

		opterr = 0;
		switch(c) {
			case 'u':
				strcpy(handle, optarg);
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
				fprintf(stderr, "使用法： %s -u [Handle] (-p [Port]) (-t [Timeout]) (-v)\n", argv[0]);
				exit(EXIT_SUCCESS);
		}
	}
	InspectE(BuildError(handle[0] == '\0', "ハンドル名不指定"));

	/* C/S決定 */
	Result r = ping(v, timeout, port);

	/* チャット */
	if (r.m == Server) {
		svrSh(v, port); /* Server動作 */
	} else {
		cliSh(v, r.ip, port, handle); /* Client動作 */
	}
}
