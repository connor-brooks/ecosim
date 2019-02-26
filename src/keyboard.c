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

  /* if key is alphabet*/
  k_event->ch = ((key > ALPHA_MIN) && (key < ALPHA_MAX)) ? 
    key : 
    NO_KEY_DATA;
  /* If key should be caps */
  k_event->ch = (k_event->ch && !(k_event->with_shift)) ? 
    key + ALOW_OFFSET : 
    k_event->ch;

  /* If key still isn't set, it should be a number or a symbol */
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
  /* Convert any numerical chars into ints */
  k_event->as_int = ((k_event->ch > CINT_MIN) & (k_event->ch < CINT_MAX)) ? 
      k_event->ch - CHAR_INT_OFFSET :
      NO_INT_DATA;


  /* If the key is a special key */
  if(key > 255)
  {
    int sk = NO_KEY_DATA;
    switch(key){
      case ESC:
        sk = ESC;
        break;

      case BACKSPACE:
        sk = BACKSPACE;
        break;

      case RETURN:
        sk = RETURN;
        break;
      default:
        sk = 0;
    };
    k_event->special = sk;
  }
  return k_event;
}

