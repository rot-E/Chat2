#include "Error.h"

const char ERROR_MESSAGE_OUTSET[] = "[エラー] ";
const char ERROR_MESSAGE_INDENT[] = "      ";

static void *DynamicMemoryAllocateE(const size_t size) {
	void *pointer = malloc(size);
	if (pointer == NULL) {
		fprintf(stderr, "%s%s 動的メモリ確保に失敗%s", "\x1b[31m", ERROR_MESSAGE_OUTSET, "\x1b[39m");
		exit(EXIT_FAILURE);
	}
	return pointer;
}

static bool IsError(Error *self) {
	return self->_Error;
}

static void SetMessage(Error *self, const char *message) {
	self->_Message = (char *)(DynamicMemoryAllocateE(
		sizeof(char) * (strlen(message) + 1)
	));
	strcpy(self->_Message, message);
}

static char *GetMessage(Error *self) {
	return self->_Message;
}

static void Handle(Error *self, const void (* handler)(void *args), void *args) {
	if (!self->IsError(self)) return;

	handler(args);
}

static void ExitIfError(Error *self) {
	if (!self->IsError(self)) return;

	void handler() {
		fprintf(stderr, "%s%s%s%s\n", "\x1b[31m", ERROR_MESSAGE_OUTSET, self->GetMessage(self), "\x1b[39m");
		self->Delete(self);
		exit(EXIT_FAILURE);
	}
	self->Handle(self, handler, NULL);
}

static void Delete(Error *self) {
	free(self->_Message);
	free(self);
}

extern Error *NewError(const bool error) {
	Error *err = (Error *)(DynamicMemoryAllocateE(sizeof(Error)));

	err->IsError		= IsError;
	err->SetMessage		= SetMessage;
	err->GetMessage		= GetMessage;

	err->Handle			= Handle;
	err->ExitIfError	= ExitIfError;

	err->Delete			= Delete;

	err->_Error			= error;
	err->_Message		= NULL;

	return err;
}

extern Error *BuildError(const bool error, const char *message) {
	Error *err = NewError(error);
	err->SetMessage(err, message);

	return err;
}
