#include "Console.h"

static void SetState(String_t *escapeSequence) {
	Console.Write(escapeSequence);
}

static void SetColour(String_t *colour) {
	Console.SetState(colour);
}

static void SetDefaultColour() {
	Console.SetState(String.New(ESCAPE_SEQUENCE.TEXT.DEFAULT));
}

static void CarriageReturn() {
	Console.Write(String.New("\r"));
}

static void Write(String_t *string) {
	printf("%s", string->Unpack(string));
	fflush(stdout);
}

static void WriteLine(String_t *string) {
	Console.Write(String.NewFormat("%s%c", string->Unpack(string), String.CHARSET.LF));
}

static void WriteColourLine(String_t *colour, String_t *string) {
	Console.SetColour(colour);
	Console.WriteLine(string);
	Console.SetDefaultColour();
}

static void WriteErrorLine(String_t *string) {
	fprintf(stderr, "%s\n", string->Unpack(string));
	fflush(stdout);
}

static void Erase() {
	Console.SetState(String.New("\e[2m"));
}

static void ErasePrevLine() {
	Console.SetState(String.New("\e[1F"));
	Console.SetState(String.New("\e[0J"));
	Console.SetState(String.New("\e[1T"));
	Console.WriteLine(String.New(""));
}

_Console Console = {
	.COLOUR = {
		.DEFAULT			= "\e[39m",
		.BLACK				= "\e[30m",
		.RED				= "\e[31m",
		.GREEN				= "\e[32m",
		.YELLOW				= "\e[33m",
		.BLUE				= "\e[34m",
		.MAGENTA			= "\e[35m",
		.CYAN				= "\e[36m",
		.LIGHT_GRAY			= "\e[37m",
		.DARK_GRAY			= "\e[90m",
		.LIGHT_RED			= "\e[91m",
		.LIGHT_GREEN		= "\e[92m",
		.LIGHT_YELLOW		= "\e[93m",
		.LIGHT_BLUE			= "\e[94m",
		.LIGHT_MAGENTA		= "\e[95m",
		.LIGHT_CYAN			= "\e[96m",
		.WHITE				= "\e[97m",	
	},

	.SetState						= SetState,
	.SetColour						= SetColour,
	.SetDefaultColour				= SetDefaultColour,

	.CarriageReturn					= CarriageReturn,

	.Write							= Write,
	.WriteLine						= WriteLine,
	.WriteColourLine				= WriteColourLine,

	.Erase							= Erase,
	.ErasePrevLine					= ErasePrevLine,
};
