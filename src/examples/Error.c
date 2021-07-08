/**
 * Error.aの使用例
 */

// make -C ../Error
// gcc Error.c ../Error.a -o Error
// ./Error

#include "../SPEC/ESCAPE_SEQUENCE.h"
#include "../Error/Error.h"

void main() {
	/* Error.New */
	Error_t *err = Error.New(false);
	if (err->IsError(err)) {
		printf("エラー\n");
	} else {
		printf("非エラー\n");
	}
	Error.Delete(err);


	err = Error.New(true);
	if (err->IsError(err)) {
		printf("エラー\n");
	} else {
		printf("非エラー\n");
	}
	Error.Delete(err);


	err = Error.New(true);
	err->Handle(err, ({
		void handler() {
			printf("エラーハンドリング\n");
		}
		handler;
	}));
	Error.Delete(err);


	/* Error.Build */
	err = Error.Build(false, "ここに理由");
	err->ExitIfError(err); // 通過
	Error.Delete(err);

	err = Error.Build(true, "ここに理由");
	err->ExitIfError(err); // 引っ掛かって終了
	Error.Delete(err);


	/* デフォルト値の変更 */
	Error.ERROR_MESSAGE_COLOUR = "\x1b[32m"; // エラーメッセージの文字色を変更
	Error.ERROR_MESSAGE_OUTSET = "! "; // エラーメッセージ先頭の文を変更
	Error.DYNAMIC_MEMORY_ALLOCATE_E.HANDLER = ({
		void my_handler() {
			printf("%s%smy_handler%s",
				ESCAPE_SEQUENCE.TEXT.BLUE,
				Error.ERROR_MESSAGE_OUTSET,
				ESCAPE_SEQUENCE.TEXT.DEFAULT
			);
			exit(EXIT_FAILURE);
		}
		my_handler;
	});
}
