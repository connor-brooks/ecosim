#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "agents.h"
#include "graphics.h"

struct _Keyboard_event {
  int ch;
  int as_int;
  int with_shift;
  int special;
};
typedef struct _Keyboard_event Keyboard_event;

Keyboard_event* keyboard_enc_event(int key, int mod);

#endif
