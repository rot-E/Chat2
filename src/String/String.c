#include "String.h"

static uint8_t *Unpack(String_t *self) {
	return self->_String;
}

static int32_t GetLength(String_t *self) {
	return strlen(self->_String);
}

static uint8_t_E GetCharAt(String_t *self, const int32_t index) {
	if (self->GetLength(self) < index) return (uint8_t_E){ Error.Build(true, "位置指定値が文字列長を超過") };

	return (uint8_t_E){ Error.New(false), self->Unpack(self)[index] };
}

static uint8_t_E GetHeadChar(String_t *self) {
	if (self->GetLength(self) < 1) return (uint8_t_E){ Error.Build(true, "文字列長が0以下") };

	return self->GetCharAt(self, 0);
}

static uint8_t_E GetLastChar(String_t *self) {
	if (self->GetLength(self) < 1) return (uint8_t_E){ Error.Build(true, "文字列長が0以下") };

	return self->GetCharAt(self, self->GetLength(self) - 1);
}

static int32_t_F FirstIndexOf(String_t *self, const uint8_t ch) {
	for (int32_t i = 0; i < self->GetLength(self); i++) {
		uint8_t_E ui8E = self->GetCharAt(self, i);
			if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else { Error.Delete(ui8E.E); return (int32_t_F){ Failure.Build(true, INTERNAL_ERROR) }; }
		uint8_t t = ui8E.V;

		if (t == ch) {
			return (int32_t_F){ Failure.New(false), i };
		}
	}

	return (int32_t_F) { Failure.Build(true, TARGET_NOT_FOUND) };
}

static int32_t_F LastIndexOf(String_t *self, const uint8_t ch) {
	for (int32_t i = self->GetLength(self); 0 <= i; i--) {
		uint8_t_E ui8E = self->GetCharAt(self, i);
			if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else { Error.Delete(ui8E.E); return (int32_t_F){ Failure.Build(true, INTERNAL_ERROR) }; }
		uint8_t t = ui8E.V;

		if (t == ch) {
			return (int32_t_F){ Failure.New(false), i };
		}
	}

	return (int32_t_F) { Failure.Build(true, TARGET_NOT_FOUND) };
}

static uint8_t_ptr_E Substring(String_t *self, const int32_t beginIndex, const int32_t lastIndex) {
	if (self->GetLength(self) + 1 < beginIndex) return (uint8_t_ptr_E){ Error.Build(true, "先頭位置指定値が異常") };
	if (self->GetLength(self) + 1 < lastIndex) return (uint8_t_ptr_E){ Error.Build(true, "終端位置指定値が異常") };
	if (beginIndex > lastIndex) return (uint8_t_ptr_E){ Error.Build(true, "不正な位置指定値") };

	uint8_t *str = (uint8_t *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(1 + lastIndex - beginIndex, sizeof(uint8_t))
	);

	strncpy(str, self->_String + beginIndex, lastIndex - beginIndex);
	str[lastIndex - beginIndex - 1] = String.CHARSET.Null;

	return (uint8_t_ptr_E){ Error.New(false), str };
}

static uint8_t_ptr_E DropLastChar(String_t *self) {
	return self->Substring(self, 0, self->GetLength(self));
}

static uint8_t_ptr_E ReplaceWithChar(String_t *self, const uint8_t oldChar, const uint8_t newChar) {
	uint8_t *str = (uint8_t *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(self->GetLength(self) + 1, sizeof(uint8_t))
	);

	for (int32_t i = 0; i < self->GetLength(self); i++) {
		uint8_t_E ui8E = self->GetCharAt(self, i);
			if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else return (uint8_t_ptr_E){ ui8E.E };
		uint8_t t = ui8E.V;

		str[i] = (t != oldChar)? t : newChar;
	}

	return (uint8_t_ptr_E){ Error.New(false), str };
}

static uint8_t_ptr_E Concat(String_t *self, String_t *str) {
	uint8_t *s = (uint8_t *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(self->GetLength(self) + str->GetLength(str) + 1, sizeof(uint8_t))
	);

	strncpy(s, self->Unpack(self), self->GetLength(self));
	strncat(s, str->Unpack(str), str->GetLength(str));

	return (uint8_t_ptr_E){ Error.New(false), s };
}

static uint8_t_ptr_E ConcatChar(String_t *self, const uint8_t ch) {
	uint8_t *s = (uint8_t *)(
		Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(1 + self->GetLength(self) + 1, sizeof(uint8_t))
	);

	char str[] = { ch };

	strncpy(s, self->Unpack(self), self->GetLength(self));
	strncat(s, str, strlen(str));

	return (uint8_t_ptr_E){ Error.New(false), s };
}

static bool IsEmpty(String_t *self) {
	return self->GetLength(self) == 0;
}

static bool Equals(String_t *self, String_t *anString) {
	return !strncmp(
		self->Unpack(self),
		anString->Unpack(anString),
		(self->GetLength(self) >= anString->GetLength(anString))?
			self->GetLength(self) : anString->GetLength(anString)
	) == 1;
}

static bool_E StartsWith(String_t *self, String_t *prefix) {
	if (self->GetLength(self) < prefix->GetLength(prefix)) return (bool_E){ Error.Build(true, "接頭辞長が文字列長を超過") };

	for (uint32_t i = 0; i < prefix->GetLength(prefix); i++) {
		uint8_t_E ui8E = self->GetCharAt(self, i);
			if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else { Error.Delete(ui8E.E); return (bool_E){ ui8E.E }; }
		char t = ui8E.V;

		ui8E = prefix->GetCharAt(prefix, i);
			if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else { Error.Delete(ui8E.E); return (bool_E){ ui8E.E }; }
		char p = ui8E.V;

		if (t != p) {
			return (bool_E){ Error.New(false), false };
		}
	}

	return (bool_E){ Error.New(false), true };
}

static bool_E StartsWithChar(String_t *self, const uint8_t ch) {
	uint8_t_E ui8E = self->GetHeadChar(self);
		if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else { Error.Delete(ui8E.E); return (bool_E){ ui8E.E }; }
	uint8_t t = ui8E.V;

	return (bool_E){ Error.New(false), t == ch };
}

static bool_E EndsWith(String_t *self, String_t *suffix) {
	if (self->GetLength(self) < suffix->GetLength(suffix)) return (bool_E){ Error.Build(true, "接尾辞長が文字列長を超過") };

	return (bool_E){
		Error.New(false),
		!strncmp(
			self->_String + self->GetLength(self) - suffix->GetLength(suffix),
			suffix->Unpack(suffix),
			self->GetLength(self)
		) == 1
	};
}

static bool_E EndsWithChar(String_t *self, const uint8_t ch) {
	uint8_t_E ui8E = self->GetLastChar(self);
		if (!ui8E.E->IsError(ui8E.E)) Error.Delete(ui8E.E); else { Error.Delete(ui8E.E); return (bool_E){ ui8E.E }; }
	uint8_t t = ui8E.V;

	return (bool_E){ Error.New(false), t == ch };
}


static String_t *New(const uint8_t *string) {
	String_t *str = (String_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.NewADT(sizeof(String_t)));

	str->Unpack					= Unpack;
	str->GetLength				= GetLength;
	str->GetCharAt				= GetCharAt;
	str->GetHeadChar			= GetHeadChar;
	str->GetLastChar			= GetLastChar;
	str->FirstIndexOf			= FirstIndexOf;
	str->LastIndexOf			= LastIndexOf;
	str->Substring				= Substring;
	str->DropLastChar			= DropLastChar;
	str->ReplaceWithChar		= ReplaceWithChar;
	str->Concat					= Concat;
	str->ConcatChar				= ConcatChar;
	str->IsEmpty				= IsEmpty;
	str->Equals					= Equals;
	str->StartsWith				= StartsWith;
	str->StartsWithChar			= StartsWithChar;
	str->EndsWith				= EndsWith;
	str->EndsWithChar			= EndsWithChar;

	str->_Size					= (string != NULL)? strlen(string) + 1 : 1;
	str->_String				= (uint8_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(str->_Size, sizeof(uint8_t)));
	if (string != NULL) strncpy(str->_String, string, strlen(string));
	str->_String[(string != NULL)? str->_Size - 1 : 0] = String.CHARSET.Null;

	return str;
}

static String_t *NewN(const size_t size) {
	String_t *str = String.New(NULL);
	String.Release(str);

	str->_Size = size;
	str->_String = (uint8_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(str->_Size, sizeof(uint8_t)));

	return str;
}

static String_t *NewFormat(const uint8_t *format, ...) {
	String_t *str = NewN(String.NEW_FORMAT_MAX_ALLOCATION_SIZE); // 要実装

	va_list ap;

	va_start(ap, format);
	vsprintf(str->_String, format, ap);
	va_end(ap);

	str->_String[str->_Size - 1] = String.CHARSET.Null; //
	String.Reduce(str);

	return str;
}

static E Reduce(String_t *str) {
	uint8_t *tmp = (uint8_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(str->GetLength(str) + 1, sizeof(uint8_t)));
	strncpy(tmp, str->Unpack(str), str->GetLength(str) + 1);

	str->_Size = str->GetLength(str) + 1;
	String.Release(str);
	str->_String = (uint8_t *)(Error.DYNAMIC_MEMORY_ALLOCATE_E.COUNTED_ALLOCATE(str->_Size, sizeof(uint8_t)));
	strncpy(str->_String, tmp, str->_Size);
	str->_String[str->_Size - 1] = String.CHARSET.Null;

	free(tmp);

	return Error.New(false);
}

static void Release(String_t *str) {
	free(str->_String);
}

static void Delete(String_t *str) {
	String.Release(str);
	free(str);
}


_String String = {
	.NEW_FORMAT_MAX_ALLOCATION_SIZE		= 100000,

	.CHARSET = {
		.Null							= '\0',
		.CR								= '\r',
		.LF								= '\n',
	},

	.New								= New,
	.NewN								= NewN,
	.NewFormat							= NewFormat,
	.Reduce								= Reduce,
	.Release							= Release,
	.Delete								= Delete,
};
