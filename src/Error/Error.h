#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "../SPEC/ESCAPE_SEQUENCE.h"

typedef struct Error_t {
	bool _Error;
	uint8_t *_Message;

	bool (* IsError)(struct Error_t *);
	uint8_t *(* GetMessage)(struct Error_t *);

	void (* Handle)(struct Error_t *, const void (* handler)());
	void (* ExitIfError)(struct Error_t *);
} Error_t;


typedef struct {
	uint8_t *ERROR_MESSAGE_COLOUR;
	uint8_t *ERROR_MESSAGE_OUTSET;
	uint8_t *ERROR_MESSAGE_INDENT;

	struct {
		uint8_t *ERROR_MESSAGE;
		void (* HANDLER)();
		void *(* ALLOCATE)(const size_t size);
		void *(* COUNTED_ALLOCATE)(const size_t num, const size_t size);
		void *(* SHARED_ALLOCATE)(const size_t size);
		void *(* NewADT)(const size_t size);
		void *(* NewSharedADT)(const size_t size);
	} DYNAMIC_MEMORY_ALLOCATE_E;

	Error_t *(* New)(const bool error);
	Error_t *(* Build)(const bool error, const uint8_t *message);
	void (* Release)(Error_t *);
	void (* Delete)(Error_t *);
} _Error;

extern _Error Error;


typedef Error_t *E;

#define return_E_NOERROR();							\
	return Error.New(false);

#define return_E(MSG);								\
	return Error.Build(true, MSG);

#define return_E_IfError(COND, MSG, ID);			\
	Error_t *err ## ID = Error.Build(COND, MSG);	\
	if (!err ## ID->IsError(err ## ID))				\
		err ## ID->Delete(err ## ID);				\
	else											\
		return err ## ID;

#define return_XE_NOERROR(XE, VAR);					\
	return (XE){ Error.Build(false), VAR };

#define return_XE(XE, MSG);							\
	return (XE){ Error.Build(true, MSG) };

#define return_XE_IfError(XE, COND, MSG, ID);		\
	Error_t *err ## ID = Error.Build(COND, MSG);	\
	if (!err ## ID->IsError(err ## ID))				\
		err ## ID->Delete(err ## ID);				\
	else											\
		return (XE){ err ## ID };
