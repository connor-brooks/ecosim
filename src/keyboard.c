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

int
keyboard_num_to_norm_buff(Normal_buffer* norm_buff, int num)
{
  if(norm_buff->cmd_id == NO_BUFF_DATA)
    return keyboard_cat_num(&(norm_buff->multiplier), num);
  else
    return keyboard_cat_num(&(norm_buff->arg), num);
}

int
keyboard_cat_num(int* to_int, int num)
{
  int tmp = *(to_int) * 10 + num;
  if(tmp > INT_MAX / 2) return 0;
  if(tmp < INT_MIN / 2)  return 0;
  if(*to_int) *to_int = tmp;
  else *to_int = num;
  return 1;
}

int
keyboard_cmd_to_norm_buff(Normal_buffer* norm_buff, int cmd)
{
  if(norm_buff->cmd_id == NO_BUFF_DATA)
    keyboard_cat_num(&(norm_buff->cmd_id), cmd);
  else
    printf("too many\n");
  printf("got cmd %d\n", norm_buff->cmd_id);
}

void
keyboard_clr_norm_buffer(Keyboard* keyb)
{
  printf("badness\n");
  keyb->norm_buff->arg = NO_BUFF_DATA;
  keyb->norm_buff->multiplier= NO_BUFF_DATA;
  keyb->norm_buff->cmd_id= NO_BUFF_DATA;
  memcpy(keyb->uig->cmd_txt, str_buffer_clear, sizeof(str_buffer_clear));
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
  Normal_buffer* norm_buff = keyb->norm_buff;

  memcpy(keyb->uig->cmd_txt, str_normal_mode, sizeof(str_normal_mode));
  switch(enc_key){
    case DEC_SHIFT(';'): /* Aka ':' */
      memcpy(keyb->uig->cmd_txt, str_insert_mode, sizeof(str_insert_mode));
      keyboard_set_mode(keyb, INSERT);
      break;

    case ESC:
      keyboard_clr_norm_buffer(keyb);
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
      // Add to own function
      if(!keyboard_num_to_norm_buff(norm_buff, enc_key - CHAR_INT_OFFSET))
        keyboard_clr_norm_buffer(keyb);
      printf("in norm buf %d\n", norm_buff->multiplier);
      break;

    case 'C':
      printf("Change\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_CHANGE);

      break;

    case 'D':
      printf("Delete\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_DELETE);
      break;

    case 'I':
      printf("Insert\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_INSERT);
      break;

    case 'P':
      printf("Paste\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_PASTE);
      break;

    case 'Q':
      printf("Quit\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_QUIT);
      break;

    case 'R':
      printf("Reload\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_RELOAD);
      break;

    case 'Y':
      printf("Yank\n");
      keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_YANK);
      break;

    default:
      memcpy(keyb->uig->cmd_txt, str_bad_key, sizeof(str_bad_key));
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
      memcpy(keyb->uig->cmd_txt, str_normal_mode, sizeof(str_normal_mode));
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
