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
#define BACKSPACE (259)
#define SHIFT (344)

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

/* Add numbers to the normal buffer. Due to syntax of the keybinds,
 * this should only happen at the start and end of a command,
 * if there is a command in normal buffer (a letter) then the number
 * should sit in the arg buffer, otherwise it should go into the
 * mutliplier.
 * If the number being inputted into either of the buffers grows too
 * large, then 0 is returned, causing the buffers to be cleared because
 * the keybind command is invalid.
 * */
int
keyboard_num_to_norm_buff(Normal_buffer* norm_buff, int num)
{
  if(norm_buff->cmd_id == NO_BUFF_DATA)
    return keyboard_cat_num(&(norm_buff->multiplier), num);
  else
    return keyboard_cat_num(&(norm_buff->arg), num);
}

/* Add numbers into the buffer,in a base 10 way. For example inputting
 * two '1's into any number buffer should create the integer 11
 * if there is anything close to a integer overflow 0 is returned */
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

/* Add the keyboard command to the normal buffer if there is no previous
 * value, otherwise return 0 */
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

/* Returns what event the key press has caused, running to
 * function appropriate to the mode*/
int
keyboard_act_result(Keyboard* keyb, int key, int mod)
{
  int act_result = KEYB_ACT_TXT_UPDATE;
  int shft_enc_key = ENC_SHIFT(key, mod);
  act_result = keyb->key_act_mode[keyb->mode](keyb, shft_enc_key);
  return act_result;
}

int
keyboard_mode_normal(Keyboard* keyb, int enc_key)
{
  Normal_buffer* norm_buff = keyb->norm_buff;

  /* Default action result is to just update text */
  int act_result = KEYB_ACT_TXT_UPDATE;

  /* What parameter of the buffer did the action change? */
  int act_cmd = NO_BUFF_DATA;
  int act_num = NO_BUFF_DATA;

  // Set default text
  memcpy(keyb->out_txt, str_normal_mode, sizeof(str_normal_mode));
  printf("commad is m=%d, c=%d, a=%d\n", norm_buff->multiplier, norm_buff->cmd_id, 
          norm_buff->arg);

  //Switch on keys
  switch(enc_key){
    case SHIFT:
      /* Nothing */
      break;

    case DEC_SHIFT(';'): /* Aka ':' */
      memcpy(keyb->out_txt, str_insert_mode, sizeof(str_insert_mode));
      keyboard_set_mode(keyb, INSERT);
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case ESC:
      keyboard_clr_norm_buffer(keyb);
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case RETURN:
      memcpy(keyb->out_txt, str_execute_cmd, sizeof(str_execute_cmd));
      printf("commad is m=%d, c=%d, a=%d\n", norm_buff->multiplier, norm_buff->cmd_id, 
          norm_buff->arg);
      act_result = KEYB_ACT_CMD_EXEC;
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
      act_num = enc_key - CHAR_INT_OFFSET;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case 'C':
      printf("Change\n");
      act_cmd = KEYB_CMD_CHANGE;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case 'D':
      printf("Delete\n");
      act_cmd = KEYB_CMD_DELETE;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case 'I':
      printf("Insert\n");
      act_cmd = KEYB_CMD_INSERT;
      act_result =  KEYB_ACT_TXT_UPDATE;
      break;

    case 'P':
      printf("Paste\n");
      act_cmd = KEYB_CMD_PASTE;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case 'Q':
      printf("Quit\n");
      act_cmd = KEYB_CMD_QUIT;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case 'R':
      printf("Reload\n");
      act_cmd = KEYB_CMD_RELOAD;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    case 'Y':
      printf("Yank\n");
      act_cmd = KEYB_CMD_YANK;
      act_result = KEYB_ACT_TXT_UPDATE;
      break;

    default:
      memcpy(keyb->out_txt, str_bad_key, sizeof(str_bad_key));
      act_result = KEYB_ACT_TXT_UPDATE;
      break;
  };
  /* If a command has been made, add it to the buffer, if
   * the buffer is already full, clear the buffer */
  if(act_cmd != NO_BUFF_DATA)
    if(!keyboard_cmd_to_norm_buff(norm_buff, act_cmd))
      keyboard_clr_norm_buffer(keyb);

  if(act_num != NO_BUFF_DATA)
    if(!keyboard_num_to_norm_buff(norm_buff, act_num))
      keyboard_clr_norm_buffer(keyb);

  return act_result;
}

int
keyboard_mode_insert(Keyboard* keyb, int enc_key)
{
  int printable_ch;

  switch(enc_key){
    case ESC:
      memcpy(keyb->out_txt, str_normal_mode, sizeof(str_normal_mode));
      keyboard_set_mode(keyb, NORMAL);
      return KEYB_ACT_TXT_UPDATE;
      break;

    default:
      printable_ch = keyboard_key_to_alpha(enc_key);
      if(printable_ch){
        size_t o_txt_len = strlen(keyb->out_txt);
        keyb->out_txt[o_txt_len++] = printable_ch;
        keyb->out_txt[o_txt_len] = '\0';
        printf("%c\n", printable_ch);
      }
      return KEYB_ACT_TXT_UPDATE;
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
