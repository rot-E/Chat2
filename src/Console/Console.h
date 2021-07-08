#pragma once

#include <stdint.h>

#include "../SPEC/ESCAPE_SEQUENCE.h"
#include "../String/String.h"

typedef struct {
	struct {
		uint8_t *DEFAULT;
		uint8_t *BLACK;
		uint8_t *RED;
		uint8_t *GREEN;
		uint8_t *YELLOW;
		uint8_t *BLUE;
		uint8_t *MAGENTA;
		uint8_t *CYAN;
		uint8_t *LIGHT_GRAY;
		uint8_t *DARK_GRAY;
		uint8_t *LIGHT_RED;
		uint8_t *LIGHT_GREEN;
		uint8_t *LIGHT_YELLOW;
		uint8_t *LIGHT_BLUE;
		uint8_t *LIGHT_MAGENTA;
		uint8_t *LIGHT_CYAN;
		uint8_t *WHITE;
	} COLOUR;

	void (* SetState)(String_t *escapeSequence);
	void (* SetColour)(String_t *colour);
	void (* SetDefaultColour)();

	void (* CarriageReturn)();

	void (* Write)(String_t *string);
	void (* WriteLine)(String_t *string);
	void (* WriteColourLine)(String_t *colour, String_t *string);

	void (* WriteErrorLine)(String_t *string);

	void (* Erase)();
	void (* ErasePrevLine)();
} _Console;

extern _Console Console;

//TODO: 表示文字列を組合させていく感じに...
/*
	View_t view = View.New();
	view.Append(String.New("test"));
	view.NextLine();
	view.AppendEscSeqed(
		ESCAPE_SEQUENCE.TEXT.RED,
		NewString("test")
	);
	view.AppendEscSeqed(
		ESCAPE_SEQUENCE.SET.BLINK,
		NewString("test")
	);
	.
	.
	.
	Console.Flush(v);
*/
