#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


Rectangle* 
create_rectangle(float x1, float y1, float x2, float y2)
{
  Rectangle* tmp = malloc(sizeof(Rectangle));

  /* Set points and return rectangle*/
  tmp->p1.x = x1;
  tmp->p1.y = y1;
  tmp->p2.x = x2;
  tmp->p2.y = y2;
  return tmp;
}

RGB*
create_RGB(float r, float g, float b)
{
  /* Set color and return */
  RGB* tmp = malloc(sizeof(RGB));
  tmp->r = r;
  tmp->g = g;
  tmp->b = b;
  return tmp;
}
