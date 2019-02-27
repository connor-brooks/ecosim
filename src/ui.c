#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "keyboard.h"
#define MAX_BUFF_LEN (64)

typedef struct RGB_ RGB;


Ui*
ui_setup(void)
{
  Ui* tmp = malloc(sizeof(Ui));

  tmp->mode = UI_MODE_NORM;
  tmp->buff = malloc(sizeof(char) * MAX_BUFF_LEN);
  tmp->buff_len = 0;
  tmp->last_out_msg = 0;
  tmp->selection[0] = -0.5f;
  tmp->selection[1] = 0.5f;
  tmp->selection[2] = 0.0f;
  tmp->selection[3] = 0.0f;
  strcpy(tmp->buff, "\0");

  return tmp;
}

Ui_resp*
ui_get_resp(Ui* ui, Keyboard_event* key_ev)
{
  Ui_resp* resp = malloc(sizeof(Ui_resp));
  int ch = key_ev->ch;

  /* UI testing functions
   * Very messy, just for experimentation */
  if(key_ev->as_int == -1){
    if(ch == 'h') ui->selection[0] -=0.05;
    if(ch == 'H') ui->selection[0] -=0.01;

    if(ch == 'j') ui->selection[1] -=0.05;
    if(ch == 'J') ui->selection[1] -=0.01;

    if(ch == 'l') ui->selection[0] +=0.05;
    if(ch == 'L') ui->selection[0] +=0.01;

    if(ch == 'k') ui->selection[1] +=0.05;
    if(ch == 'K') ui->selection[1] +=0.01;

    ui_msg_buff_any(ui->buff, &ui->buff_len, "Select size", &ui->last_out_msg);

    resp->code = UI_RESP_UPDATE_TEXT + UI_RESP_SEL_MODE;
    resp->selection = ui->selection;
    resp->buff_txt = ui->buff;
  }
  else
  {
    ui_cat_to_buff_any(ch, ui->buff, &ui->buff_len, &ui->last_out_msg);
    resp->code = UI_RESP_UPDATE_TEXT;
    resp->buff_txt = ui->buff;
  }
  return resp;
}

/* cat char to buffer */
int
ui_cat_to_buff_any(int ch, char* buff, size_t *buff_len, int* last_out_msg)
{
  /* Instant clear if the previous buffer write was from a msg */
  if((*last_out_msg))
  {
    ui_clear_buff_any(buff, buff_len);
    *last_out_msg = 0;
  }

  /* If there is space, add the char to the buffer */
  if((*buff_len) + 1 < MAX_BUFF_LEN)
  {
    size_t dest_len = strlen(buff);
    buff[dest_len++] = ch;
    buff[dest_len] = '\0';
    (*buff_len)++;
    return 1;
  }

  /* Otherwise send a warning to the buffer */
  else
  {
    ui_msg_buff_any(buff, buff_len, "Buffer too full!", last_out_msg);
    return 0;
  }
}

/* Write null-byte to buffer */
int
ui_clear_buff_any(char* buff, size_t* buff_len){
  char blank[] = "\0";
  *buff_len = strlen(blank);
  strcpy(buff, blank);
  return 0;
}

/* Clear the null-byte and the previous letter */
int
ui_backspace_buff_any(char* buff, size_t *buff_len){
  size_t dest_len = strlen(buff);
  buff[dest_len--] = '\0';
  buff[dest_len--] = '\0';
  *buff_len = strlen(buff);
}

/* Copy a message to the buffer */
int
ui_msg_buff_any(char* buff, size_t *buff_len, char* msg, int* last_out_msg)
{
  /* Stop printing same msg */
  if(strcmp(buff, msg))  printf("%s\n", msg);

  /* Copy to buffer */
  *buff_len = sizeof(msg);
  strcpy(buff, msg);
  *last_out_msg = 1;
}
