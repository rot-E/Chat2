#pragma once

#include <stdint.h>

/** ANSI Escape Sequence
 *
 * cf. https://misc.flogisoft.com/bash/tip_colors_and_formatting
 * cf. https://en.wikipedia.org/wiki/ANSI_escape_code
 * cf. https://qiita.com/PruneMazui/items/8a023347772620025ad6
 */

typedef struct {
	struct {
		uint8_t *BOLD;
		uint8_t *DIM;
		uint8_t *UNDERLINED;
		uint8_t *BLINK;
		uint8_t *REVERSE;
		uint8_t *HIDDEN;
	} SET;

	struct {
		uint8_t *ALL;
		uint8_t *BOLD;
		uint8_t *DIM;
		uint8_t *UNDERLINED;
		uint8_t *BLINK;
		uint8_t *REVERSE;
		uint8_t *HIDDEN;
	} RESET;

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
	} TEXT;

	struct {
		uint8_t *DEFAULT;
		uint8_t *BLACK;
		uint8_t *RED;
		uint8_t	*GREEN;
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
	} BACKGROUND;
} _ESCAPE_SEQUENCE;

extern _ESCAPE_SEQUENCE ESCAPE_SEQUENCE;
