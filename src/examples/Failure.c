/**
 * Error.aの使用例
 */

// make -C ../Failure
// gcc Failure.c ../Error.a -o Failure
// ./Failure

#include "../Error/Failure.h"

void main() {
	/* Failure.New */
	Failure_t *fail = Failure.New(false);
	if (fail->IsFailed(fail)) {
		printf("失敗\n");
	} else {
		printf("失敗せず\n");
	}
	Failure.Delete(fail);

	/* Failure.Build */
	fail = Failure.Build(true, 3);
	if (fail->IsFailed(fail)) {
		printf("%d\n", fail->GetCaseNum(fail));

		fail->Handle(fail, ({
			void handler(const int32_t caseNum) {
				switch (caseNum) {
					case 1:
						printf("handler: 1\n");
						break;

					case 2:
						printf("handler: 2\n");
						break;

					case 3:
						printf("handler: 3\n");
						break;

					default:
						printf("handler: x\n");
						break;
				}
			};
			handler;
		}));
	}
	Failure.Delete(fail);
}
