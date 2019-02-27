#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "keyboard.h"

#define PRINTABLE_MIN (31)
#define PRINTABLE_MAX (127)
#define ALPHA_MIN (64)
#define ALPHA_MAX (91)
#define CINT_MIN (47)
#define CINT_MAX (58)

#define ALOW_OFFSET (32)
#define CHAR_INT_OFFSET (48)

#define ESC (256)
#define RETURN (257)
#define BACKSPACE (259)
#define SHIFT (344)

#define ENC_SHIFT(k, m) (k + (m << 16))
#define DEC_SHIFT(c) (c + (1 << 16))

#define NO_KEY_DATA (0)
#define NO_INT_DATA (-1)

Keyboard_event*
keyboard_enc_event(int key, int mod)
{
  Keyboard_event* k_event = malloc(sizeof(Keyboard_event));
  k_event->ch = NO_KEY_DATA;
  k_event->as_int= NO_INT_DATA;
  k_event->special= NO_KEY_DATA;
  k_event->with_shift= mod;

  /* if key is alphabetical, keep put ot ch */
  k_event->ch = ((key > ALPHA_MIN) && (key < ALPHA_MAX)) ?
    key :
    NO_KEY_DATA;

  /* If alphabetical hasn't been shifted, shift it to lower */
  k_event->ch = (k_event->ch && !(k_event->with_shift)) ?
    key + ALOW_OFFSET :
    k_event->ch;

  /* If key still isn't set, it should be a number or a symbol,
   * more work is needed. */
  if((!k_event->ch) && (key > PRINTABLE_MIN) && (key < PRINTABLE_MAX))
  {
    /* Encrypt the key with it's mod for use in switch */
    int enc_key = ENC_SHIFT(key, mod);
    int k = NO_KEY_DATA;
    switch(enc_key){
      case DEC_SHIFT(';'):
        k = ':';
        break;
      case DEC_SHIFT('1'):
        k = '!';
        break;
      case DEC_SHIFT('2'):
        k = '"';
        break;
      case DEC_SHIFT('3'):
        k = '#';
        break;
      case DEC_SHIFT('4'):
        k = '$';
        break;
      case DEC_SHIFT('5'):
        k = '%';
        break;
      default:
        k = key;

    };
    k_event->ch = k;
  }
  /* Convert any numerical chars into ints for any math needed */
  k_event->as_int = ((k_event->ch > CINT_MIN) & (k_event->ch < CINT_MAX)) ?
    k_event->ch - CHAR_INT_OFFSET :
    NO_INT_DATA;


  /* If the key is a special key, save it to special */
  if(key > 255)
  {
    int sk = NO_KEY_DATA;
    switch(key){
      case ESC:
      case BACKSPACE:
      case RETURN:
        sk = key;
        break;
      default:
        sk = NO_KEY_DATA;
    };
    k_event->special = sk;
  }
  return k_event;
}

