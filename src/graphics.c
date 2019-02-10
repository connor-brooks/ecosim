#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "utils.h"

void 
drawRectangle(Rectangle* rectangle, RGB* color)
{
  glBegin(GL_QUADS);
  glVertex2f(rectangle->topLeft.x, rectangle->topLeft.y);
  glVertex2f(rectangle->bottomRight.x, rectangle->topLeft.y);
  glVertex2f(rectangle->bottomRight.x, rectangle->bottomRight.y);
  glVertex2f(rectangle->topLeft.x, rectangle->bottomRight.y);
  glEnd();
}

