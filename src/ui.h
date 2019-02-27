#ifndef UI_H
#define UI_H
#include "keyboard.h"
#include "ui.h"
#include "ui_response.h"

enum ui_modes {
  UI_MODE_NORM,
  UI_MODE_INSERT,
  UI_MODE_SELECT
};

struct _Ui {
  int mode;
  int last_out_msg;
  char* buff;
  size_t buff_len;
  float selection[4];
};

typedef struct _Ui Ui;

Ui* ui_setup(void);

Ui_resp* ui_get_resp(Ui* ui, Keyboard_event* key_ev);

/* All these should use the UI struct instead of passing values like this */
int ui_cat_to_buff_any(int ch, char* buff, size_t *buff_len, int* last_out_msg);
int ui_clear_buff_any(char* buff, size_t *buff_len);
int ui_backspace_buff_any(char* buff, size_t *buff_len);
int ui_msg_buff_any(char* buff, size_t *buff_len, char* msg, int* last_out_msg);

/* add function to take command response, probabl from cmd_resp struct? */
#endif
