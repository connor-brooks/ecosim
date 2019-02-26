#ifndef UI_H
#define UI_H
#include "keyboard.h"
#include "ui.h"

enum ui_modes {
  UI_MODE_NORM,
  UI_MODE_INSERT,
  UI_MODE_SELECT
};

enum ui_resp {
  UI_RESP_NONE = 0,
  UI_RESP_UPDATE_TEXT = 1,
  UI_RESP_RUN_CMD = 2,
  UI_RESP_SEL_MODE = 4
};


struct _Ui {
  int mode;
  int resp;
  char* tmp_buff;
  size_t tmp_buff_len;
  char* out_buff;
  int last_out_msg;
  size_t out_buff_len;
  float sel_x1;
  float sel_y1;
  float sel_x2;
  float sel_y2;
};
typedef struct _Ui Ui;

Ui* ui_setup(void);
Ui* ui_get_resp(Ui* ui, Keyboard_event* key_ev);
/* Neaten these up to use the struct */
int ui_cat_to_buff_any(int ch, char* buff, size_t *buff_len, int* last_out_msg);
int ui_clear_buff_any(char* buff, size_t *buff_len);
int ui_backspace_buff_any(char* buff, size_t *buff_len);
int ui_msg_buff_any(char* buff, size_t *buff_len, char* msg, int* last_out_msg);
#endif
