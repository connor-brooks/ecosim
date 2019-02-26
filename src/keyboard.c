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
    return keyboard_cat_num(&(norm_buff->cmd_id), cmd);
  else
    return 0;
  printf("got cmd %d\n", norm_buff->cmd_id);
}

void
keyboard_clr_norm_buffer(Keyboard* keyb)
{
  keyb->norm_buff->arg = NO_BUFF_DATA;
  keyb->norm_buff->multiplier= NO_BUFF_DATA;
  keyb->norm_buff->cmd_id= NO_BUFF_DATA;
  memcpy(keyb->out_txt, str_buffer_clear, sizeof(str_buffer_clear));
}


void
keyboard_set_mode(Keyboard* keyb, int mode)
{
  keyb->mode = mode;
}

/* Returns what event the key press has caused */
int
keyboard_act_result(Keyboard* keyb, int key, int mod)
{
  //  printf("Key %d and mod %d\n", key, mod); // For debug
  /* Default */
  int ret = KEYB_ACT_TXT_UPDATE;
  int shft_enc_key = ENC_SHIFT(key, mod);
  keyb->key_act_mode[keyb->mode](keyb, shft_enc_key);
  return ret;
}

int
keyboard_mode_normal(Keyboard* keyb, int enc_key)
{
  Normal_buffer* norm_buff = keyb->norm_buff;

  memcpy(keyb->out_txt, str_normal_mode, sizeof(str_normal_mode));
  switch(enc_key){
    case DEC_SHIFT(';'): /* Aka ':' */
      memcpy(keyb->out_txt, str_insert_mode, sizeof(str_insert_mode));
      keyboard_set_mode(keyb, INSERT);
      break;

    case ESC:
      keyboard_clr_norm_buffer(keyb);
      break;

    case RETURN:
      printf("Attempting to execute command\n");
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
      printf("in multi buf %d\n", norm_buff->multiplier);
      printf("in arg buf %d\n", norm_buff->arg);
      break;

    case 'C':
      printf("Change\n");
      if(!keyboard_cmd_to_norm_buff(norm_buff, KEYB_CMD_CHANGE))
        keyboard_clr_norm_buffer(keyb);

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
      memcpy(keyb->out_txt, str_bad_key, sizeof(str_bad_key));
      break;
  };
}

int
keyboard_mode_insert(Keyboard* keyb, int enc_key)
{
  int printable_ch;

  switch(enc_key){
    case ESC:
      memcpy(keyb->out_txt, str_normal_mode, sizeof(str_normal_mode));
      keyboard_set_mode(keyb, NORMAL);
      break;

    default:
      printable_ch = keyboard_key_to_alpha(enc_key);
      if(printable_ch){
        size_t o_txt_len = strlen(keyb->out_txt);
        keyb->out_txt[o_txt_len++] = printable_ch;
        keyb->out_txt[o_txt_len] = '\0';
        printf(":%c\n", printable_ch);
      }
      break;
  };
}

int
keyboard_mode_select(Keyboard* keyb, int enc_key)
{
  printf("I am in select mode!\n");
}

/* Will expand to properly map all the keys
 * So far this function only shifts between caps and
 * lower, need to get it working with symbols too */
int
keyboard_key_to_alpha(int key)
{
  int was_shifted = 0;
  // If the key has had shift pressed, remove shift from the key
  if(key > SHIFT_BITMASK)
  {
    was_shifted = 1;
    key = key - SHIFT_BITMASK;
  }
  // If the key wasn't shifted but is a letter key, put it in lower case
  if((key > ALPHA_MIN) & (key < ALPHA_MAX))
    key = key + ((!was_shifted) * CAPS_TO_LOW);
  return ((key > PRINTABLE_MIN) & (key < PRINTABLE_MAX))? key : BAD_KEY;
}
