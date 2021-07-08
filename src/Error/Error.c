#include "Error.h"

static bool IsError(Error_t *self) {
	return self->_Error;
}

static uint8_t *GetMessage(Error_t *self) {
	return self->_Message;
}

static void Handle(Error_t *self, const void (* handler)()) {
	if (!self->IsError(self)) return;

	handler();
}

static void ExitIfError(Error_t *self) {
	if (!self->IsError(self)) return;

	self->Handle(self, ({
		void handler() {
			fprintf(stderr, "%s%s%s%s\n",
				Error.ERROR_MESSAGE_COLOUR,
				Error.ERROR_MESSAGE_OUTSET,
				self->GetMessage(self),
				ESCAPE_SEQUENCE.TEXT.DEFAULT
			);
			Error.Delete(self);
			exit(EXIT_FAILURE);
		}
		handler;
	}));
}


static Error_t *New(const bool error) {
	Error_t *err = (Error_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.NewADT(sizeof(Error)));

	err->IsError		= IsError;
	err->GetMessage		= GetMessage;
	err->Handle			= Handle;
	err->ExitIfError	= ExitIfError;

	err->_Error			= error;
	err->_Message		= NULL;

	return err;
}

static Error_t *Build(const bool error, const uint8_t *message) {
	Error_t *err = Error.New(error);

	err->_Message = (uint8_t *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(strlen(message) + 1, sizeof(uint8_t))
	);
	strcpy(err->_Message, message);

	return err;
}

static void Release(Error_t *err) {
	free(err->_Message);
}

static void Delete(Error_t *err) {
	Error.Release(err);
	free(err);
}


static void DEFAULT_HANDLER() {
	fprintf(stderr, "%s%s%s%s\n",
		Error.ERROR_MESSAGE_COLOUR,
		Error.ERROR_MESSAGE_OUTSET,
		Error.DYNAMIC_MEMORY_ALLOCATE_E.ERROR_MESSAGE,
		ESCAPE_SEQUENCE.TEXT.DEFAULT
	);
	exit(EXIT_FAILURE);
}

static void *ALLOCATE(const size_t size) {
	void *pointer = malloc(size);
	if (pointer == NULL) Error.DYNAMIC_MEMORY_ALLOCATE_E.HANDLER();

	return pointer;
}

static void *COUNTED_ALLOCATE(const size_t num, const size_t size) {
	void *pointer = calloc(num, size);
	if (pointer == NULL) Error.DYNAMIC_MEMORY_ALLOCATE_E.HANDLER();

	return pointer;
}

static void *SHARED_ALLOCATE(const size_t size) {
	void *pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (pointer == NULL) Error.DYNAMIC_MEMORY_ALLOCATE_E.HANDLER();

	return pointer;
}

_Error Error = {
	.ERROR_MESSAGE_COLOUR						= "\x1b[31m",
	.ERROR_MESSAGE_OUTSET						= "[エラー] ",
	.ERROR_MESSAGE_INDENT						= "      ",

	.DYNAMIC_MEMORY_ALLOCATE_E = {
		.ERROR_MESSAGE							= "動的メモリ確保に失敗",
		.HANDLER								= DEFAULT_HANDLER,
		.ALLOCATE								= ALLOCATE,
		.COUNTED_ALLOCATE						= COUNTED_ALLOCATE,
		.SHARED_ALLOCATE						= SHARED_ALLOCATE,
		.NewADT									= ALLOCATE,
		.NewSharedADT							= SHARED_ALLOCATE,
	},

	.New										= New,
	.Build										= Build,
	.Release									= Release,
	.Delete										= Delete,
};
