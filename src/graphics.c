#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "utils.h"

void 
drawRectangle(Rectangle* rectangle, RGB* color)
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
