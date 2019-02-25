#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "graphics.h"
#include "config.h"
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
  glVertexAttribPointer(0, AGENT_VERT_DIMS, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribPointer(1, AGENT_VERT_DIMS, GL_FLOAT, GL_FALSE, 0, (void*) color_offset);
  glDrawArrays(GL_POINTS, 0, ag->no_agents);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);
}


void
ui_gfx_setup(Ui_graphics* uig)
{
  uig->height = 0.05;
  float tmp_v[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    1.0f, -1.0f + uig->height,
    -1.0f, -1.0f+ uig->height};
  size_t tmp_v_size = sizeof(tmp_v);

  uig->vertex_data = malloc(tmp_v_size);
  memcpy(uig->vertex_data, tmp_v, tmp_v_size);
}

void
ui_draw(Ui_graphics* uig)
{
  float* tmp_ptr = uig->vertex_data;
  glColor3f(0.5, 0.5, 0.5);
  glBegin(GL_QUADS);
  for(int i = 0; i < 4; i++, tmp_ptr += 2)
    glVertex3f(*tmp_ptr, *(tmp_ptr + 1), 0.0f);
  glEnd();
  draw_text(-1.0, -0.98,(const unsigned char *) uig->cmd_txt);
  /* Swap buffers */
}



void
draw_text(float x, float y, const unsigned char* txt)
{
  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos3d(x, y, 0.0);
  glutBitmapString(GLUT_BITMAP_9_BY_15, txt);
}


