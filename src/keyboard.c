#include "keyboard.h"
#include "graphics.h"


void 
keyboard_ui_ptr(Keyboard *keyb, Ui_graphics* uig)
{
  keyb->uig = uig;
}

void 
keyboard_action(Keyboard* keyb, int key, int mod)
{
  printf("Key %c and mod %d\n", key, mod);
  keyb->uig->cmd_txt = "Key X Pressed! :) \0";
}
