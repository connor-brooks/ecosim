#include <stdlib.h>
#include <limits.h>
#include "keyboard.h"
#include "graphics.h"
#include "string.h"

#define PRINTABLE_MIN (20)
#define PRINTABLE_MAX (127)
#define ALPHA_MIN (64)
#define ALPHA_MAX (91)
#define CAPS_TO_LOW (32)
#define CHAR_TO_INT (48)
#define ESC (256)
#define SHIFT_BITMASK (1 << 16)
#define ENC_SHIFT(k, m) (k + (m << 16))
#define DEC_SHIFT(c) (c + (1 << 16))
#define BAD_KEY (0)
#define NO_BUFF_DATA (0)

void
keyboard_setup(Keyboard* keyb)
{
  keyb->key_act_mode[NORMAL] = &keyboard_mode_normal;
  keyb->key_act_mode[INSERT] = &keyboard_mode_insert;
  keyb->key_act_mode[SELECT] = &keyboard_mode_select;
  keyb->norm_buff = keyboard_make_norm_buff();
}

Normal_buffer*
keyboard_make_norm_buff(void)
{
  Normal_buffer* tmp_norm_buff = malloc(sizeof(Normal_buffer));
  tmp_norm_buff->multiplier = NO_BUFF_DATA;
  tmp_norm_buff->cmd_id = NO_BUFF_DATA;
  tmp_norm_buff->arg = NO_BUFF_DATA;
  return tmp_norm_buff;
}

void 
keyboard_num_to_norm_buff(Normal_buffer* norm_buff, int num)
{
  if(norm_buff->cmd_id == NO_BUFF_DATA)
    keyboard_cat_num(&(norm_buff->multiplier), num);
  else
    keyboard_cat_num(&(norm_buff->arg), num);

  printf("it's %d\n", norm_buff->multiplier);
}

void 
keyboard_cat_num(int* to_int, int num)
{
  int tmp = *(to_int) * 10 + num;
  if(tmp > INT_MAX / 2) return;
  if(tmp < INT_MIN / 2)  return;
  if(*to_int) *to_int = tmp;
  else *to_int = num;

}
void 
keyboard_to_norm_buff_arg(Normal_buffer* norm_buff, int num){
//
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
  //  printf("Key %d and mod %d\n", key, mod); // For debug
  int shft_enc_key = ENC_SHIFT(key, mod);
  keyb->key_act_mode[keyb->mode](keyb, shft_enc_key);
}

void
keyboard_mode_normal(Keyboard* keyb, int enc_key)
{
  char bad_key_str[] = "Normal mode: BAD keypress!";
  char buffer_clr_str[] = "Normal mode: Buffer cleared!";
  char nrm_mode_prmpt[] = "Normal mode:";
  char ch_insert_str[] = "Insert mode:";

  memcpy(keyb->uig->cmd_txt, nrm_mode_prmpt, sizeof(nrm_mode_prmpt));

  switch(enc_key){
    case DEC_SHIFT(';'): /* Aka ':' */
      memcpy(keyb->uig->cmd_txt, ch_insert_str, sizeof(ch_insert_str));
      keyboard_set_mode(keyb, INSERT);
      break;

    case ESC:
      memcpy(keyb->uig->cmd_txt, buffer_clr_str, sizeof(buffer_clr_str));
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      printf("Number\n");
      keyboard_num_to_norm_buff(keyb->norm_buff, enc_key - CHAR_TO_INT);
      break;


    case 'C':
      printf("Change\n");
      break;

    case 'D':
      printf("Delete\n");
      break;

    case 'I':
      printf("Insert\n");
      break;

    case 'P':
      printf("Paste\n");
      break;

    case 'Q':
      printf("Quit\n");
      break;

    case 'R':
      printf("Reload\n");
      break;

    case 'Y':
      printf("Yank\n");
      break;

    default:
      memcpy(keyb->uig->cmd_txt, bad_key_str, sizeof(bad_key_str));
      break;
  };
}
void
keyboard_mode_insert(Keyboard* keyb, int enc_key)
{
  char ch_norm_str[] = "Normal mode:";
  int printable_ch;

  switch(enc_key){
    case ESC:
      memcpy(keyb->uig->cmd_txt, ch_norm_str, sizeof(ch_norm_str));
      keyboard_set_mode(keyb, NORMAL);
      break;

    default:
      printable_ch = keyboard_key_to_alpha(enc_key);
      if(printable_ch)
        printf(":%c\n", printable_ch);
      break;
  };
}
void
keyboard_mode_select(Keyboard* keyb, int enc_key)
{
  printf("I am in select mode!\n");
}

/* Will expand to properly map all the keys
 * So far this function only shifts between caps and
 * lower, need to get it working with symbols too */
int
keyboard_key_to_alpha(int the_key)
{
  int was_shifted = 0;
  // If the key has had shift pressed, remove shift from the key
  if(the_key > SHIFT_BITMASK)
  {
    was_shifted = 1;
    the_key = the_key - SHIFT_BITMASK;
  }
  // If the key wasn't shifted but is a letter key, put it in lower case
  if((the_key > ALPHA_MIN) & (the_key < ALPHA_MAX))
    the_key = the_key + ((!was_shifted) * CAPS_TO_LOW);
  return ((the_key > PRINTABLE_MIN) & (the_key < PRINTABLE_MAX))? the_key : BAD_KEY;
}