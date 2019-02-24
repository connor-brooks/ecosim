#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "graphics.h"
#include "utils.h"


void
agent_vbo_setup(Agent_graphics* ag)
{
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, ag->vert_data_len, ag->vert_data, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  ag->vbo = vbo;
}

void
agent_shader_setup(Agent_graphics* ag)
{
  GLuint vs = 0;
  GLuint fs = 0;
  GLuint shader = 0;

  vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &(ag->vert_shader), NULL);
  glCompileShader(vs);

  fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &(ag->frag_shader), NULL);
  glCompileShader(fs);

  shader = glCreateProgram();
  glAttachShader(shader, fs);
  glAttachShader(shader, vs);
  glLinkProgram(shader);
  ag->shader = shader;

}

void
agents_draw(Agent_graphics* ag)
{
  int agent_dims = 4;
  size_t color_offset = ag->no_agents * agent_dims * sizeof(float);

  glUseProgram(ag->shader);
  glBindBuffer(GL_ARRAY_BUFFER, ag->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, ag->vert_data_len, ag->vert_data);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) color_offset);
  glDrawArrays(GL_POINTS, 0, 3);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);
}


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


