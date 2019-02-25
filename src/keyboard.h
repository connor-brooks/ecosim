#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "agents.h"
#include "graphics.h"

enum keyboard_mode {
  NORMAL,
  INSERT,
  SELECT
};

struct _Keyboard {
 int mode; 
 Ui_graphics* uig;
};

typedef struct _Keyboard Keyboard;

void keyboard_ui_ptr(Keyboard* keyb, Ui_graphics* uig);

void keyboard_action(Keyboard* keyb, int key, int mod);

#endif
