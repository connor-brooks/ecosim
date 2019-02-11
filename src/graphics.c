#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "utils.h"

/* Draw rectangle from args */
void
drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b)
{
  /* Draw counter clockwiswe */
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
  glVertex2f(x1, y1);
  glVertex2f(x2, y1);
  glVertex2f(x2, y2);
  glVertex2f(x1, y2);

  glEnd();
}

/* Draw from structs */
void
drawRectangleStruct(Rectangle* rectangle, RGB* color)
{
  /* Draw counter clockwiswe */
  glColor3f(color->red, color->green, color->blue);
  glBegin(GL_QUADS);
  glVertex2f(rectangle->p1.x, rectangle->p1.y);
  glVertex2f(rectangle->p2.x, rectangle->p1.y);
  glVertex2f(rectangle->p2.x, rectangle->p2.y);
  glVertex2f(rectangle->p1.x, rectangle->p2.y);

  glEnd();
}

void
drawText(float x, float y, const unsigned char* text)
{
  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos3d(x, y, 0.0);
  glutBitmapString(GLUT_BITMAP_9_BY_15, text);
}
