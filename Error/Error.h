#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern const int ERROR_MESSAGE_OUTSET_LENGTH;
extern const char ERROR_MESSAGE_OUTSET[];
extern const char ERROR_MESSAGE_INDENT[];

typedef struct Error {
	bool _Error;
	char *_Message;

	bool (* IsError)(struct Error *);
	void (* SetMessage)(struct Error *, const char *message);
	char *(* GetMessage)(struct Error *);

	void (* Handle)(struct Error *, const void (* handler)(void *args), void *args);
	void (* ExitIfError)(struct Error *);

	void (* Delete)(struct Error *);
} Error;

typedef Error *E;

extern Error *NewError(const bool error);
extern Error *BuildError(const bool error, const char *message);
