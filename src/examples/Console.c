/**
 * Console.aの使用例
 */

// make -C ../Console
// gcc Console.c ../Console.a -o Console
// ./Console

#include "../SPEC/ESCAPE_SEQUENCE.h"
#include "../Console/Console.h"
#include "../String/String.h"

void main() {
	Console.WriteLine(String.New("test"));
	Console.SetColour(String.New(ESCAPE_SEQUENCE.TEXT.BLUE));
	Console.WriteLine(String.New("test"));
	Console.SetDefaultColour();
	Console.WriteLine(String.New("test"));

	Console.Write(String.New(ESCAPE_SEQUENCE.SET.BLINK));
	Console.Write(String.New("blink"));
	Console.Write(String.New(ESCAPE_SEQUENCE.RESET.BLINK));
	Console.WriteLine(String.New(""));

	Console.WriteLine(String.New("test1"));
	Console.WriteLine(String.New("test2"));
	Console.ErasePrevLine();
}
