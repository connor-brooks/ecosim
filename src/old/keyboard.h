#ifndef KEYBOARD_H
#define KEYBOARD_H
#define PRINTABLE_MIN (31)
#define PRINTABLE_MAX (127)
#define ALPHA_MIN (64)
#define ALPHA_MAX (91)
#define CINT_MIN (47)
#define CINT_MAX (58)

#define ALOW_OFFSET (32)
#define CHAR_INT_OFFSET (48)

#define ESC (256)
#define RETURN (257)
#define BACKSPACE (259)
#define SHIFT (344)

#define ENC_SHIFT(k, m) (k + (m << 16))
#define DEC_SHIFT(c) (c + (1 << 16))

#define NO_KEY_DATA (0)
#define NO_INT_DATA (-1)

#include "agents.h"

struct _Keyboard_event {
  int ch;
  int as_int;
  int with_shift;
  int special;
};
typedef struct _Keyboard_event Keyboard_event;

Keyboard_event* keyboard_enc_event(int key, int mod);

#endif
