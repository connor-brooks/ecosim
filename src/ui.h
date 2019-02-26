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
  UI_RESP_NONE,
  UI_RESP_UPDATE_TEXT,
  UI_RESP_RUN_CMD,
  UI_RESP_BOTH
};


struct _Ui {
  int mode;
  int resp;
  char* tmp_buff;
  size_t tmp_buff_len;
  char* out_buff;
  int last_out_msg;
  size_t out_buff_len;
};
typedef struct _Ui Ui;

Ui* Ui_setup(void);
Ui* ui_get_resp(Ui* ui, Keyboard_event* key_ev);
int ui_cat_to_buff_any(int ch, char* buff, size_t *buff_len, int* last_out_msg);
int ui_clear_buff_any(char* buff, size_t *buff_len);
int ui_backspace_buff_any(char* buff, size_t *buff_len);
#endif
