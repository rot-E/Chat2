#pragma once

#include <stdint.h>

#include <limits.h>
#include <string.h>

#include <stdarg.h>

#include "../Error/Error.h"
#include "../Error/EType.h"
#include "../Error/Failure.h"
#include "../Error/FType.h"

typedef struct String_t {
	int32_t _Size;
	uint8_t *_String;

	/* 取得系 */
	uint8_t *(* Unpack)(struct String_t *);
	int32_t (* GetLength)(struct String_t *);
	uint8_t_E (* GetCharAt)(struct String_t *, const int32_t index);
	uint8_t_E (* GetHeadChar)(struct String_t *);
	uint8_t_E (* GetLastChar)(struct String_t *);

	int32_t_F (* FirstIndexOf)(struct String_t *, const uint8_t ch);
	int32_t_F (* LastIndexOf)(struct String_t *, const uint8_t ch);

	/* 加工系 */
	uint8_t_ptr_E (* Substring)(struct String_t *, const int32_t beginIndex, const int32_t lastIndex);
	uint8_t_ptr_E (* DropLastChar)(struct String_t *);
	uint8_t_ptr_E (* ReplaceWithChar)(struct String_t *, const uint8_t oldChar, const uint8_t newChar);
	uint8_t_ptr_E (* Concat)(struct String_t *, struct String_t *str);
	uint8_t_ptr_E (* ConcatChar)(struct String_t *, const uint8_t ch);

	/* 検査系 */
	bool (* IsEmpty)(struct String_t *);

	bool (* Equals)(struct String_t *, struct String_t *anString);
	bool_E (* StartsWith)(struct String_t *, struct String_t *prefix);
	bool_E (* StartsWithChar)(struct String_t *, const uint8_t ch);
	bool_E (* EndsWith)(struct String_t *, struct String_t *suffix);
	bool_E (* EndsWithChar)(struct String_t *, const uint8_t ch);
} String_t;


typedef struct {
	int32_t NEW_FORMAT_MAX_ALLOCATION_SIZE;

	struct {
		uint8_t Null;
		uint8_t CR;
		uint8_t LF;
	} CHARSET;

	enum {
		INTERNAL_ERROR,
		TARGET_NOT_FOUND,
	} Failure;

	String_t *(* New)(const uint8_t *str);
	String_t *(* NewN)(const size_t size);
	String_t *(* NewFormat)(const uint8_t *format, ...);
	E (* Reduce)(String_t *);
	void (* Release)(String_t *);
	void (* Delete)(String_t *);
} _String;

extern _String String;


typedef struct {
	Error_t *E;
	String_t *V;
} String_t_E;

typedef struct {
	Failure_t *F;
	String_t *V;
} String_t_F;
