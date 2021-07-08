#include "Failure.h"

static bool IsFailed(Failure_t *self) {
	return self->_Failure;
}

static int32_t GetCaseNum(Failure_t *self) {
	return self->_CaseNum;	
}

static void Handle(Failure_t *self, const void (* handler)(const int32_t caseNum)) {
	if (!self->IsFailed(self)) return;

	handler(self->_CaseNum);
}


static Failure_t *New(const bool failure) {
	Failure_t *fail = (Failure_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.NewADT(sizeof(Failure_t)));

	fail->IsFailed			= IsFailed;
	fail->GetCaseNum		= GetCaseNum;
	fail->Handle			= Handle;

	fail->_Failure			= failure;
	fail->_CaseNum			= 0;

	return fail;
}

static Failure_t *Build(const bool failure, const int32_t caseNum) {
	Failure_t *fail = Failure.New(failure);

	fail->_CaseNum = caseNum;

	return fail;
}

static void Release(Failure_t *fail) {
}

static void Delete(Failure_t *fail) {
	Failure.Release(fail);
	free(fail);
}

_Failure Failure = {
	.New		= New,
	.Build		= Build,
	.Release	= Release,
	.Delete		= Delete,
};
