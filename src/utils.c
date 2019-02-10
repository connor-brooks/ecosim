#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


Rectangle* 
createRectangle(float x1, float y1, float x2, float y2)
{
  Point topLeft = {.x = x1, .y = y1};
  Point bottomRight = {.x = x2, .y = y2};
  Rectangle* temp = malloc(sizeof(Rectangle));

  /* Set points and return rectangle*/
  temp->bottomRight = bottomRight;
  temp->topLeft = topLeft;
  return temp;
}

RGB*
createRGB(int red, int green, int blue)
{
  /* Set color and return */
  RGB* temp = malloc(sizeof(RGB));
  temp->red = red;
  temp->green = green;
  temp->blue = blue;
  return temp;
}
