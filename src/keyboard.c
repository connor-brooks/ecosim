#include "keyboard.h"
#include "graphics.h"
#include "string.h"


void 
keyboard_ui_ptr(Keyboard *keyb, Ui_graphics* uig)
{
  keyb->uig = uig;
}

void 
keyboard_action(Keyboard* keyb, int key, int mod)
{
  printf("Key %c and mod %d\n", key, mod);
  char test[] = {key, key, key, '\0'};
  memcpy(keyb->uig->cmd_txt, test, sizeof(test));


}
