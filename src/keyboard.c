#include "keyboard.h"
#include "graphics.h"
#include "string.h"

#define ESC (-1)
#define ENC_SHIFT(k, m) (k + (m << 16))
#define DEC_SHIFT(c) (c + (1 << 16))

void
keyboard_setup(Keyboard* keyb)
{
  keyb->key_act_mode[NORMAL] = &keyboard_mode_normal;
  keyb->key_act_mode[INSERT] = &keyboard_mode_insert;
  keyb->key_act_mode[SELECT] = &keyboard_mode_select;
}

void
keyboard_ui_ptr(Keyboard *keyb, Ui_graphics* uig)
{
  keyb->uig = uig;
}

void
keyboard_set_mode(Keyboard* keyb, int mode)
{
  keyb->mode = mode;
}

void
keyboard_action(Keyboard* keyb, int key, int mod)
{
//  printf("Key %d and mod %d\n", key, mod);
  keyb->key_act_mode[keyb->mode](keyb, key, mod);
}

void
keyboard_mode_normal(Keyboard* keyb, int key, int mod)
{
  int shft_mod = mod << 16;
  int shft_modded_key = ENC_SHIFT(key, mod);
  switch(shft_modded_key){
    case 'A':
      printf("Normal A\n");
      break;
    case DEC_SHIFT('A'):
      printf("Shifted A\n");
      break;

    case ';':
      keyboard_set_mode(keyb, INSERT);
      break;
  };
}
void
keyboard_mode_insert(Keyboard* keyb, int key, int mod)
{
  switch(key){
    case -1:
      keyboard_set_mode(keyb, NORMAL);
      break;
  };
  printf("I am in insert mode!\n");
  char test[] = {'K', 'e', 'y', ' ', key, ' ', 'm','o', 'd', mod+48, '\0'};
  memcpy(keyb->uig->cmd_txt, test, sizeof(test));
}
void
keyboard_mode_select(Keyboard* keyb, int key, int mod){
  printf("I am in select mode!\n");
}
