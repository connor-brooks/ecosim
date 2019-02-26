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
  char* out_buff;
};
typedef struct _Ui Ui;

Ui* Ui_setup(void);
Ui* Ui_get_resp(Ui* ui, Keyboard_event* key_ev);
#endif
