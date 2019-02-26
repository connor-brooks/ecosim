#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "keyboard.h"
#define MAX_BUFF_LEN (64)


Ui*
Ui_setup(void)
{
  Ui* tmp = malloc(sizeof(Ui));
  tmp->mode = UI_MODE_NORM;
  tmp->resp = UI_RESP_NONE;
  tmp->tmp_buff = malloc(sizeof(char) * MAX_BUFF_LEN);
  tmp->out_buff = malloc(sizeof(char) * MAX_BUFF_LEN);
  tmp->out_buff_len = 0;
  tmp->tmp_buff_len = 0;
  tmp->last_out_msg = 0;
  strcpy(tmp->tmp_buff, "\0");
  strcpy(tmp->out_buff, "\0");

  return tmp;
}

Ui*
Ui_get_resp(Ui* ui, Keyboard_event* key_ev)
{
  int ch = key_ev->ch;
  /* If the key wasn't special, put it in the buffer */
  if(!key_ev->special )
    ui_cat_to_buff_any(ch, ui->out_buff, &ui->out_buff_len, &ui->last_out_msg);
  else
    ui_backspace_buff_any(ui->out_buff, &ui->out_buff_len);

  ui->resp = UI_RESP_UPDATE_TEXT;

}

int
ui_cat_to_buff_any(int ch, char* buff, size_t *buff_len, int* last_out_msg)
{
  if((*last_out_msg))
  {
    ui_clear_buff_any(buff, buff_len);
    *last_out_msg = 0;
  }

  if((*buff_len) + 1 < MAX_BUFF_LEN)
  {
    size_t dest_len = strlen(buff);
    buff[dest_len++] = ch;
    buff[dest_len] = '\0';
    (*buff_len)++;
    return 1;
  }
  else
  {
    char error[] = "Buffer too full!";
    *buff_len = sizeof(error);
    strcpy(buff, error);
    *last_out_msg = 1;
    return 0;
  }
}

int
ui_clear_buff_any(char* buff, size_t* buff_len){
  char blank[] = " ";
  *buff_len = strlen(blank);
  strcpy(buff, blank);
  return 0;
}

int
ui_backspace_buff_any(char* buff, size_t *buff_len){
  size_t dest_len = strlen(buff);
  buff[dest_len--] = '\0';
  buff[dest_len--] = '\0';
  *buff_len = strlen(buff);
}
