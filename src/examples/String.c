/**
 * String.aの使用例
 */

// make -C ../String
// gcc String.c ../Error.a -o String
// ./String

#include <stdio.h>

#include "../String/String.h"

#include "EControl.h"

void main() {
	String_t *s = String.New("test string");
	printf("s=\"%s\"   |s|=%d   s(0)='%c'   s(|s|-1)='%c'\n",
		s->Unpack(s),
		s->GetLength(s),
		UnwrapE(s->GetCharAt(s, 0)),
		UnwrapE(s->GetLastChar(s))
	);

	const uint8_t TGT	= 's';

	int32_t_F result = s->FirstIndexOf(s, TGT);
	result.F->Handle(result.F, ({
		void handler(const int32_t caseNum) {
			if (caseNum == TARGET_NOT_FOUND)
				printf("文字が見つかりませんでした: %c'\n", TGT);
				result.V = -1;
		}
		handler;
	}));
	printf("first index of 's': %d\n", result.V);

	result = s->LastIndexOf(s, 's');
	result.F->Handle(result.F, ({
		void handler(const int32_t caseNum) {
			if (caseNum == TARGET_NOT_FOUND)
				printf("文字が見つかりませんでした: '%c'\n", TGT);
				result.V = -1;
		}
		handler;
	}));
	printf("last index of 's': %d\n", result.V);

	printf("\"%s\" is empty? %s\n", s->Unpack(s),
		(s->IsEmpty(s))? "YES" : "NO"
	);

	String.Delete(s);
	s					= String.New("");
	printf("\"%s\" is empty? %s\n", s->Unpack(s),
		(s->IsEmpty(s))? "YES" : "NO"
	);

	String.Delete(s);
	s					= String.New("あいうえお aiueo");
	String_t *aiu		= String.New("あいう");
	String_t *ueo_latin	= String.New("ueo");
	printf("\"%s\" starts with \"%s\"? %s\n", s->Unpack(s), "あいう",
		( UnwrapE(s->StartsWith(s, aiu)) )? "YES" : "NO"
	);
	printf("\"%s\" starts with \"%s\"? %s\n", s->Unpack(s), "ueo",
		( UnwrapE(s->StartsWith(s, ueo_latin)) )? "YES" : "NO"
	);
	printf("\"%s\" ends with \"%s\"? %s\n", s->Unpack(s), "あいう",
		( UnwrapE(s->EndsWith(s, aiu)) )? "YES" : "NO"
	);
	printf("\"%s\" ends with \"%s\"? %s\n", s->Unpack(s), "ueo",
		( UnwrapE(s->EndsWith(s, ueo_latin)) )? "YES" : "NO"
	);
	String.Delete(aiu);
	String.Delete(ueo_latin);

	String_t *aiueo		= String.New("あいうえお aiueo");
	printf("\"%s\" equals \"%s\"? %s\n", s->Unpack(s), s->Unpack(s),
		(s->Equals(s, s))? "YES" : "NO"
	);
	printf("\"%s\" equals \"%s\"? %s\n", s->Unpack(s), aiueo->Unpack(aiueo),
		(s->Equals(s, aiueo))? "YES" : "NO"
	);
	aiueo				= String.New("あいうえお aiueo ");
	printf("\"%s\" equals \"%s\"? %s\n", s->Unpack(s), aiueo->Unpack(aiueo),
		(s->Equals(s, aiueo))? "YES" : "NO"
	);
	aiueo				= String.New("あいうえお aiueo ");
	printf("\"%s\" equals \"%s\"? %s\n", aiueo->Unpack(aiueo), s->Unpack(s),
		(aiueo->Equals(aiueo, s))? "YES" : "NO"
	);

	String.Delete(s);
	s				= String.New("aiueotttttX");
	printf("Original: %s\n", s->Unpack(s));
	printf("DropLastChar: %s\n", UnwrapE(s->DropLastChar(s)));
	printf("Substring['0-2'3]: %s\n", UnwrapE(s->Substring(s, 0, 3))); // マルチバイト/ワイド文字は使用不能
	printf("Substring[2'3-4'5]: %s\n", UnwrapE(s->Substring(s, 3, 5)));
	printf("t -> X: %s\n", UnwrapE(s->ReplaceWithChar(s, 't', 'X')));

	printf("s#s: %s\n", UnwrapE(s->Concat(s, s)));
	printf("s#kakikukeko: %s\n", UnwrapE(s->Concat(s, String.New("kakikukeko"))));

	String.Delete(s);
	s = String.NewN(100);
	printf("before: %s\n", s->Unpack(s));
	strcpy(s->Unpack(s), "外部からの代入");
	String.Reduce(s);
	printf("after: %s\n", s->Unpack(s));

	String.Delete(s);
	s = String.NewFormat("%d %d %s %c %f", 3, 2, "test test", 'z', 3.14);
	printf("%s\n", s->Unpack(s));

	String.New(NULL); // chars[0] = '\0'
}
