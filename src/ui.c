#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "keyboard.h"


Ui*
Ui_setup(void)
{
  Ui* tmp = malloc(sizeof(Ui));
  tmp->mode = UI_MODE_NORM;
  tmp->resp = UI_RESP_NONE;
  tmp->tmp_buff = malloc(sizeof(char) * 64);
  tmp->out_buff = malloc(sizeof(char) * 64);
  strcpy(tmp->tmp_buff, " ");
  strcpy(tmp->out_buff, " ");

  return tmp;
}

Ui* 
Ui_get_resp(Ui* ui, Keyboard_event* key_ev)
{
  int a = key_ev->as_int;
  if(key_ev->as_int != -1 ) strcat(ui->out_buff, "int ");
  else if(key_ev->ch) strcat(ui->out_buff, "char ");
  if(key_ev->special) strcat(ui->out_buff, "special ");

  printf("Got number %d\n", a);
//  strcpy(ui->out_buff, string);
  ui->resp = UI_RESP_UPDATE_TEXT;
  
}
