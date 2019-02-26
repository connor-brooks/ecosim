#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "keyboard.h"
#include "graphics.h"
#include "strings.h"

#define PRINTABLE_MIN (20)
#define PRINTABLE_MAX (127)
#define ALPHA_MIN (64)
#define ALPHA_MAX (91)
#define CAPS_TO_LOW (32)
#define CHAR_INT_OFFSET (48)

#define ESC (256)
#define RETURN (257)
#define BACKSPACE (259)
#define SHIFT (344)

#define SHIFT_BITMASK (1 << 16)
#define ENC_SHIFT(k, m) (k + (m << 16))
#define DEC_SHIFT(c) (c + (1 << 16))
#define BAD_KEY (0)
#define NO_BUFF_DATA (0)

