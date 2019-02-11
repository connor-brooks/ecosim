#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


Rectangle* 
create_rectangle(float x1, float y1, float x2, float y2)
{
  Rectangle* temp = malloc(sizeof(Rectangle));

  /* Set points and return rectangle*/
  temp->p1.x = x1;
  temp->p1.y = y1;
  temp->p2.x = x2;
  temp->p2.y = y2;
  return temp;
}

RGB*
create_RGB(float red, float green, float blue)
{
  /* Set color and return */
  RGB* temp = malloc(sizeof(RGB));
  temp->red = red;
  temp->green = green;
  temp->blue = blue;
  return temp;
}
