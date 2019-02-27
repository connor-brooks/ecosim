#ifndef UI_RESPONSE_H
#define UI_RESPONSE_H

enum ui_resp_code {
  UI_RESP_NONE        = 0,
  UI_RESP_UPDATE_TEXT = 1 << 0,
  UI_RESP_RUN_CMD     = 2 << 1,
  UI_RESP_SEL_MODE    = 4 << 2
    /*UI_RESP_SEL_RETURN = 8? */
};

struct _Ui_resp{
  int code;
  float selection[4];
  char* buff_txt;
};

typedef struct _Ui_resp Ui_resp;
#endif
