#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "utils.h"

/* Draw rectangle from args */
void
draw_rectangle(float x1, float y1, float x2, float y2, float r, float g, float b)
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
draw_rectangle_struct(Rectangle* rect, RGB* c)
{
  /* Draw counter clockwiswe */
  glColor3f(c->r, c->g, c->b);
  glBegin(GL_QUADS);
  glVertex2f(rect->p1.x, rect->p1.y);
  glVertex2f(rect->p2.x, rect->p1.y);
  glVertex2f(rect->p2.x, rect->p2.y);
  glVertex2f(rect->p1.x, rect->p2.y);

  glEnd();
}

void
draw_text(float x, float y, const unsigned char* txt)
{
  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos3d(x, y, 0.0);
  glutBitmapString(GLUT_BITMAP_9_BY_15, txt);
}
