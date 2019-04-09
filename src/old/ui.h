#ifndef UI_H
#define UI_H
#include "keyboard.h"
#include "ui.h"
#include "ui_response.h"

enum ui_modes {
  UI_MODE_NORM,
  UI_MODE_SELECT
};

enum ui_sel_dirs {
  UI_SEL_AXIS_X,
  UI_SEL_AXIS_Y
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
void ui_free(Ui* ui);

Ui_resp* ui_get_resp(Ui* ui, Keyboard_event* key_ev);

void ui_resp_norm_mode(Ui* ui, Keyboard_event* key_ev, Ui_resp* resp);
void ui_resp_sel_mode(Ui* ui, Keyboard_event* key_ev, Ui_resp* resp);

int ui_cat_to_buff(int ch, Ui* ui);
int ui_clear_buff(Ui* ui);
int ui_backspace_buff(Ui* ui);
int ui_msg_buff(Ui* ui, char* msg);

void ui_resize_sel(float* cur, float diff);

/* add function to take command response, probabl from cmd_resp struct? */
#endif
