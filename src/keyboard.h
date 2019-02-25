#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "agents.h"
#include "graphics.h"

enum keyboard_mode {
  NORMAL,
  INSERT,
  SELECT
};

typedef struct _Keyboard Keyboard;

struct _Keyboard {
 int mode; 
 Ui_graphics* uig;
 void (*key_act_mode[3])(Keyboard* keyb, int key, int mod);
};

void keyboard_setup(Keyboard* keyb);

void keyboard_ui_ptr(Keyboard* keyb, Ui_graphics* uig);

void keyboard_set_mode(Keyboard* keyb, int mode);

void keyboard_action(Keyboard* keyb, int key, int mod);

void keyboard_mode_normal(Keyboard* keyb, int key, int mode);
void keyboard_mode_insert(Keyboard* keyb, int key, int mode);
void keyboard_mode_select(Keyboard* keyb, int key, int mode);


#endif
