/**
 * Error.aの使用例
 */

// make -C ../Error
// gcc Error.c ../Error.a -o Error
// ./Error

#include "../Error/Error.h"

void main() {
	/* NewError */
	Error *err = NewError(false);
	if (err->IsError(err)) {
		printf("エラー\n");
	} else {
		printf("非エラー\n");
	}
	err->Delete(err);


	err = NewError(true);
	if (err->IsError(err)) {
		printf("エラー\n");
		err->SetMessage(err, "かくかくしかじか");
		printf("%s\n", err->GetMessage(err)); // SetMessageしていなければ、GetMessageでSIGSEGV
	} else {
		printf("非エラー\n");
	}
	err->Delete(err);


	/* BuildError */
	err = BuildError(false, "ここに理由");
	err->ExitIfError(err); // 通過
	err->Delete(err);


	err = BuildError(true, "ここに理由");
	err->ExitIfError(err); // 引っ掛かって終了
	err->Delete(err);
}
