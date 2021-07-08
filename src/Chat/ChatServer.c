#include "ChatServer.h"

/**
 * チャットサーバ処理を行う函数
 */
static Error * Start(ChatServer *self) {
	Error *err;

	printf("[情報] %d番ポートで待受中\n", self->_Port); fflush(stdout);
	const int socket_listen = init_tcpserver(self->_Port, 5);

	typedef struct {
		int Socket;
		char *Handle;
		bool Free;
	} User;
	User user[PARTICIPANT_MAX_N];
	for (int i = 0; i < PARTICIPANT_MAX_N; i++)
		user[i].Free = true;

	fd_set FDState;
	FD_ZERO(&FDState);

	long max_fd = 0;

	/* ログイン処理スレッド用函数 */
	int participant = 0; // TODO: 以下でこの値ではなく、最小空き位置を求めて使用するようにして可参加化
	void * join(void *_args) {
		char str[DATA_MAX_SIZE];
		int result;

		for (;;) {
			join_top:
			memset(str, '\0', sizeof(str));

			if (participant >= PARTICIPANT_MAX_N) {
				/* 参加人数上限に達している場合 */ // TODO: 人口減による可参加化
				int socket = accept(socket_listen, NULL, NULL);

				// 受信(Login.Request無視)
				result = recv(socket, str, DATA_MAX_SIZE - 1, 0);
					err = NewError(result == -1);
					if (err->IsError(err)) {
						err->SetMessage(err, "recv failed.");
						fprintf(stderr, "%s%s\n", ERROR_MESSAGE_OUTSET, err->GetMessage(err));  fflush(stderr);

						goto join_top;
					}

				// エラー送信
				memset(str, '\0', sizeof(str));

				sprintf(str, "Login.Failed/参加人数が上限に達しています%s", CRLF);
				result = send(socket, str, strlen(str), 0);
					err = NewError(result == -1);
					if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }

				// 切断
				close(socket);
			} else {
				/* 参加可能の場合 */ // TODO:退出→参加可能に対応させる
				user[participant].Socket = accept(socket_listen, NULL, NULL);
				printf("[接続/入場] #%d\n", participant); fflush(stdout);

				/* "Login.Request/{handle_name}" を受信 */
				int result = recv(user[participant].Socket, str, DATA_MAX_SIZE - 1, 0);
					err = NewError(result == -1);
					if (err->IsError(err)) {
						err->SetMessage(err, "recv failed.");
						fprintf(stderr, "%s%s\n", ERROR_MESSAGE_OUTSET, err->GetMessage(err));  fflush(stderr);

						goto join_top;
					}
				str[strlen(str) + 1] = '\0';
				printf("[受信/入場] #%d %s", participant, str); fflush(stdout);

				// handle_name切り出し
				char str_1[DATA_MAX_SIZE];
				strcpy(str_1, str);
				strtok(str_1, "/");
				char *hdl = strtok(NULL, "/");
					err = NewError(hdl == NULL);
					if (err->IsError(err)) {
						err->SetMessage(err, "handle取り出し失敗");
						fprintf(stderr, "%s%s\n", ERROR_MESSAGE_OUTSET, err->GetMessage(err)); fflush(stderr);

						goto join_top;
					}
				strtok(hdl, CRLF);

				// handle_nameが既使用の場合弾く
				for (int i = 0; i < participant; i++) {
					if (user[i].Free) continue;

					if (!strcmp(user[i].Handle, hdl)) {
						memset(str, '\0', sizeof(str));

						sprintf(str, "Login.Failed/The handle %s is already used.%s", hdl, CRLF);
						result = send(user[participant].Socket, str, strlen(str), 0);
							err = NewError(result == -1);
							if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
						close(user[participant].Socket);
						printf("[送信/入場] #%d %s", participant, str); fflush(stdout);

						goto join_top;
					}
				}

				user[participant].Handle = (char *)(malloc(sizeof(char) * (strlen(hdl) + 1)));
					err = NewError(user[participant].Handle == NULL);
					if (err->IsError(err)) { err->SetMessage(err, "malloc failed."); return err; }
				strcpy(user[participant].Handle, hdl);
				printf("[情報/入場] #%d handle is %s\n", participant, user[participant].Handle); fflush(stdout);

				/* ログイン成功通知送信 */
				memset(str, '\0', sizeof(str));

				sprintf(str, "Login.Succeeded%s", CRLF);
				result = send(user[participant].Socket, str, strlen(str), 0);
					err = NewError(result == -1);
					if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
				printf("[送信/入場] #%d %s", participant, str); fflush(stdout);

				/* 存在管理 */
				FD_SET(user[participant].Socket, &FDState);
				max_fd = (user[participant].Socket > max_fd)? user[participant].Socket : max_fd;

				user[participant].Free = false;
				participant++;

				/* 入場全体報知 */ // TODO: 実装
				/*memset(str, '\0', sizeof(str));

				for (int j = 0; j <= participant; j++) {
					if (user[j].Free) continue;
					if (j == participant) continue;

					sprintf(str, "Login/%s%s", user[participant].Handle, CRLF);
					result = send(user[j].Socket, str, strlen(str), 0);
						err = NewError(result == -1);
						if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
					printf("[送信/入場] #%d %s", j, str); fflush(stdout);
				}*/
			}
		}
	}

	/* 書込・退出処理スレッド用函数 */
	void * chat(void *_args) {
		for (;;) {
			fd_set tmp = FDState;

			select(max_fd + 1, &tmp, NULL, NULL, &(struct timeval){ .tv_sec = 0, .tv_usec = 0 });

			for (int i = 0; i <= participant; i++) {
				if (user[i].Free) continue;

				if (FD_ISSET(user[i].Socket, &tmp)) {
					/* 変化あり */
					char str[DATA_MAX_SIZE];
					int result;

					// 受信
					result = recv(user[i].Socket, str, DATA_MAX_SIZE - 1, 0);
						err = NewError(result == -1);
						if (err->IsError(err)) {
							err->SetMessage(err, "recv failed.");
							fprintf(stderr, "%s%s\n", ERROR_MESSAGE_OUTSET, err->GetMessage(err)); fflush(stderr);

							continue;
						}

						/* 切断検知 */
						err = NewError(result == 0);
						if (err->IsError(err)) {
							/* 情報削除 */
							FD_CLR(user[i].Socket, &FDState);

							close(user[i].Socket);

							free(user[i].Handle);
							user[i].Free = true;

							fprintf(stderr, "[情報] #%d 切断\n", i);

							continue;
						}
					str[strlen(str) + 1] = '\0';
					printf("[受信/書込・退出] #%d %s", i, str); fflush(stdout);

					if (strstr(str, "Write.Request") != NULL) {
						/* 書き込み処理 "Write.Request/{message}" 受信 */
						// message切り出し
						char str_1[DATA_MAX_SIZE];
						strcpy(str_1, str);
						strtok(str_1, "/");
						char *msg = strtok(NULL, "/");
							err = NewError(msg == NULL);
							if (err->IsError(err)) {
								err->SetMessage(err, "message取り出し失敗");
								fprintf(stderr, "%s%s\n", ERROR_MESSAGE_OUTSET, err->GetMessage(err)); fflush(stderr);

								/* エラー通知送信 */
								memset(str, '\0', sizeof(str));

								sprintf(str, "Write.Failed/internal error%s", CRLF);
								result = send(user[i].Socket, str, strlen(str), 0);
									err = NewError(result == -1);
									if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
								printf("[送信/書込] #%d %s", i, str); fflush(stdout);

								memset(str, '\0', sizeof(str));

								continue;
							}
						strtok(msg, CRLF);

						// 書き込み成功通知送信
						memset(str, '\0', sizeof(str));

						sprintf(str, "Write.Succeeded%s", CRLF);
						result = send(user[i].Socket, str, strlen(str), 0);
							err = NewError(result == -1);
							if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
						printf("[送信/書込] #%d %s", i, str); fflush(stdout);

						// 全体送信
						memset(str, '\0', sizeof(str));

						sprintf(str, "Message/%s/%s%s", user[i].Handle, msg, CRLF);

						for (int j = 0; j <= participant; j++) {
							if (user[j].Free) continue;

							result = send(user[j].Socket, str, strlen(str), 0);
								err = NewError(result == -1);
								if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
							printf("[送信/書込] #%d %s", j, str); fflush(stdout);
						}

						memset(str, '\0', sizeof(str));
					} else if (strstr(str, "Logout.Request") != NULL) {
						/* 退出処理 "Logout.Request" 受信 */
						/* ログアウト通知送信 */
						memset(str, '\0', sizeof(str));

						sprintf(str, "Logout.Succeeded%s", CRLF);
						result = send(user[i].Socket, str, strlen(str), 0);
							err = NewError(result == -1);
							if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
						printf("[送信/退出] #%d %s", i, str); fflush(stdout);

						/* 退出全体報知 */ // TODO: 実装
						/*memset(str, '\0', sizeof(str));

						for (int j = 0; j <= participant; j++) {
							if (user[j].Free) continue;
							if (j == i) continue;

							sprintf(str, "Logout/%s%s", user[i].Handle, CRLF);
							result = send(user[j].Socket, str, strlen(str), 0);
								err = NewError(result == -1);
								if (err->IsError(err)) { err->SetMessage(err, "send failed."); return err; }
							printf("[送信/退出] #%d %s", j, str); fflush(stdout);
						}*/

						/* 情報削除 */
						FD_CLR(user[i].Socket, &FDState);

						close(user[i].Socket);

						free(user[i].Handle);
						user[i].Free = true;
					}
				}
			}
		}
	}

	pthread_t tid_join;
	int result = pthread_create(&tid_join, NULL, join, NULL);
		err = NewError(result != 0);
		if (err->IsError(err)) { err->SetMessage(err, "pthread_create failed."); return err; }

	pthread_t tid_chat;
	result = pthread_create(&tid_chat, NULL, chat, NULL);
		err = NewError(result != 0);
		if (err->IsError(err)) { err->SetMessage(err, "pthread_create failed."); return err; }

	for (;;) sleep(1); // 取り敢えずmainスレッドはここで止める TODO: mainスレッドをmain函数に返してstop呼出でサーバ機能停止(強制ログアウト実装)

	return NewError(false);
}

/**
 * ChatServerの構築を行う函数
 */
extern Return_NewChatServer * NewChatServer(const int port) {
	Error *err;

	ChatServer *svr = (ChatServer *)(malloc(sizeof(ChatServer)));
		err = NewError(svr == NULL);
		if (err->IsError(err)) { err->SetMessage(err, DYNAMIC_MEMORY_ALLOCTE_ERROR_MESSAGE); return &(Return_NewChatServer){ err }; }

	svr->Start = Start;


	svr->_Port = port;

	return &(Return_NewChatServer){ NewError(false), svr };
}
