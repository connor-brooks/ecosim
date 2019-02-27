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
  tmp->selection[0] = -0.1f;
  tmp->selection[1] = 0.5f;
  tmp->selection[2] = 0.0f;
  tmp->selection[3] = 0.0f;
  strcpy(tmp->buff, "\0");

  return tmp;
}

/* Return ui response */
Ui_resp*
ui_get_resp(Ui* ui, Keyboard_event* key_ev)
{
  Ui_resp* resp = malloc(sizeof(Ui_resp));

  if(key_ev->special){
    if(key_ev->special == RETURN)
    {
      ui->mode = !ui->mode;
      printf("mode %d\n", ui->mode);
    }
  }
  switch(ui->mode){
    case UI_MODE_NORM:
      ui_resp_norm_mode(ui, key_ev, resp);
      break;
    case UI_MODE_SELECT:
      ui_resp_sel_mode(ui, key_ev, resp);
      break;
  };
  return resp;
}

/* responding in normal mode */
void
ui_resp_norm_mode(Ui* ui, Keyboard_event* key_ev, Ui_resp* resp)
{
  int ch = key_ev->ch;
  ui_cat_to_buff(ch, ui);
  resp->code = UI_RESP_UPDATE_TEXT;
  resp->buff_txt = ui->buff;
}

/* responding in select mode */
void
ui_resp_sel_mode(Ui* ui, Keyboard_event* key_ev, Ui_resp* resp)
{
  int ch = key_ev->ch;
  if(ch == 'h') ui->selection[0] -=0.05;
  if(ch == 'H') ui->selection[0] -=0.01;

  if(ch == 'j') ui->selection[1] -=0.05;
  if(ch == 'J') ui->selection[1] -=0.01;

  if(ch == 'l') ui->selection[0] +=0.05;
  if(ch == 'L') ui->selection[0] +=0.01;

  if(ch == 'k') ui->selection[1] +=0.05;
  if(ch == 'K') ui->selection[1] +=0.01;

  ui_msg_buff(ui, "Select Size");

  resp->code = UI_RESP_UPDATE_TEXT + UI_RESP_SEL_MODE;
  resp->selection = ui->selection;
  resp->buff_txt = ui->buff;
}

/* cat char to buffer */
int ui_cat_to_buff(int ch, Ui* ui)
{
  /* Instant clear if the previous buffer write was from a msg */
  if(ui->last_out_msg)
  {
    ui_clear_buff(ui);
    ui->last_out_msg = 0;
  }

  /* If there is space, add the char to the buffer */
  if(ui->buff_len + 1 < MAX_BUFF_LEN)
  {
    size_t dest_len = strlen(ui->buff);
    ui->buff[dest_len++] = ch;
    ui->buff[dest_len] = '\0';
    ui->buff_len++;
    return 1;
  }

  /* Otherwise send a warning to the buffer */
  else
  {
    ui_msg_buff(ui, "Buffer too full!");
    return 0;
  }
}

/* Write null-byte to buffer */

int
ui_clear_buff(Ui* ui){
  char blank[] = "\0";
  ui->buff_len = strlen(blank);
  strcpy(ui->buff, blank);
  return 0;
}


/* Clear the null-byte and the previous letter */
int
ui_backspace_buff(Ui* ui){
  if(ui->last_out_msg) ui_clear_buff(ui);
  else{
    size_t dest_len = strlen(ui->buff);
    ui->buff[dest_len--] = '\0';
    ui->buff[dest_len--] = '\0';
    ui->buff_len = strlen(ui->buff);
  }
}

/* Copy a message to the buffer */

int
ui_msg_buff(Ui* ui, char* msg)
{
  /* Stop printing same msg */
  if(strcmp(ui->buff, msg))  printf("%s\n", msg);

  /* Copy to buffer */
  strcpy(ui->buff, msg);
  ui->buff_len = sizeof(msg);
  ui->last_out_msg = 1;
}
