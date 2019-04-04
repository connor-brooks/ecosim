#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "graphics.h"
#include "agents.h"
#include "quadtree.h"
#include "config.h"
#include "utils.h"
#include "ui_response.h"

GLuint 
gfx_setup_shader(const char* vs_raw, const char* fs_raw)
{
  GLuint vs = 0;
  GLuint fs = 0;
  GLuint shader = 0;
  vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &(vs_raw), NULL);
  glCompileShader(vs);

  fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &(fs_raw), NULL);
  glCompileShader(fs);

  shader = glCreateProgram();
  glAttachShader(shader, fs);
  glAttachShader(shader, vs);
  glLinkProgram(shader);
  return shader;

}

GLuint 
gfx_agent_shader()
{
  const char* agents_vs =
    "#version 130\n"
    "attribute vec4 position;"
    "attribute vec4 color;"
    "uniform vec2 window;"
    "out vec4 color_out;"
    "void main() {"
    "color_out = color;"
    "  gl_Position = vec4(position.x, position.y, position.z, 1.0);"
    "  gl_PointSize = position.w * window.x;"
    "}";

  const char* agents_fs =
    "#version 130\n"
    "out vec4 frag_colour;"
    "in vec4 color_out;"
//    "float random(vec4 col)"
//    "{"
//    "return fract(sin(dot(col,"
//    "vec4(12.123213, 32.2312, 213.2312, 87.8343))));"
//    "}"
    "void main() {"
    " vec2 test2 = gl_PointCoord - vec2(0.5);"
 //   " float rand = random(color_out) * 0.2;"
    " if(length(test2) > 0.5) discard;"
    "  gl_FragColor = color_out * (1 - (length(test2) ));"
    //"  gl_FragColor = color_out ;"
    "}";

  return gfx_setup_shader(agents_vs, agents_fs);

}

GLuint 
gfx_agent_vis_shader()
{
  const char* vis_vs =
    "#version 130\n"
    "attribute vec4 position;"
    "attribute vec4 color;"
    "uniform vec2 window;"
    "out vec4 color_out;"
    "void main() {"
    "color_out = color;"
    "  gl_Position = vec4(position.x, position.y, position.z, 1.0);"
    "  gl_PointSize = position.w * window.x * 400;"
    "}";

  const char* vis_fs =
    "#version 130\n"
    "out vec4 frag_colour;"
    "in vec4 color_out;"
//    "float random(vec4 col)"
//    "{"
//    "return fract(sin(dot(col,"
//    "vec4(12.123213, 32.2312, 213.2312, 87.8343))));"
//    "}"
    "void main() {"
    " vec2 test2 = gl_PointCoord - vec2(0.5);"
 //   " float rand = random(color_out) * 0.2;"
    " if(length(test2) > 0.5) discard;"
    "  gl_FragColor = color_out * (length(test2) * 1.0) + 0.05;"
    " if(color_out.w == 0) gl_FragColor.w = 0;"
//    "  gl_FragColor = color_out ;"
    "}";

  return gfx_setup_shader(vis_vs, vis_fs);

}

Ui_graphics*
ui_gfx_setup(void)
{
  Ui_graphics* tmp = malloc(sizeof(Ui_graphics));

  tmp->height = 0.05;
  tmp->show_sel = 0;
  float tmp_v[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    1.0f, -1.0f + tmp->height,
    -1.0f, -1.0f+ tmp->height};
  size_t tmp_v_size = sizeof(tmp_v);
  tmp->cmd_txt = malloc(sizeof(char) * 64);
  strcpy(tmp->cmd_txt, " ");

  tmp->vertex_data = malloc(tmp_v_size);
  memcpy(tmp->vertex_data, tmp_v, tmp_v_size);
  return tmp;
}


void 
ui_gfx_free(Ui_graphics* uig)
{
//
free(uig->vertex_data);
free(uig->cmd_txt);
free(uig);
}

void
ui_draw(Ui_graphics* uig)
{
  float* tmp_ptr = uig->vertex_data;
  glColor4f(0.7, 0.7, 0.7, 0.5);
  glBegin(GL_QUADS);

  for(int i = 0; i < 4; i++, tmp_ptr += 2) {
    glVertex3f(*tmp_ptr, *(tmp_ptr + 1), 0.0f);
    //printf("i is at %d\n", i);
  }
  glEnd();

  gfx_text_draw(-0.98, -0.98,(const unsigned char *) uig->cmd_txt);

  /* Draw the selector (temp bit of code) */
  if(uig->show_sel)
  {
    glColor3f(0.5, 0.5, 0.9);
    glBegin(GL_LINE_LOOP);
    glVertex3f(uig->selection[0], uig->selection[1], 0.0);
    glVertex3f(uig->selection[2], uig->selection[1], 0.0);
    glVertex3f(uig->selection[2], uig->selection[3], 0.0);
    glVertex3f(uig->selection[0], uig->selection[3], 0.0);
    glEnd();
  }
}

void
ui_gfx_update(Ui_resp* resp, Ui_graphics* uig)
{
  /* Update text from UI */
  if(resp->code && (UI_RESP_UPDATE_TEXT || UI_RESP_RUN_CMD))
    strcpy(uig->cmd_txt, resp->buff_txt);

  /* Update select drwaing */
  if(resp->code & UI_RESP_SEL_MODE )
  {
    uig->show_sel = 1;
    memcpy(uig->selection, resp->selection, sizeof(float) * 4);
  }
  else uig->show_sel = 0;
}


void
gfx_text_draw(float x, float y, const unsigned char* txt)
{
  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos3d(x, y, 0.0);
  glutBitmapString(GLUT_BITMAP_9_BY_15, txt);
}


void
gfx_quad_draw(Quadtree_verts* qv)
{
  glColor4f(0.1, 0.1, 0.1, 0.5);
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, qv->verts);
  glDrawArrays(GL_QUADS, 0, qv->q_count * 4);
  glDisableClientState(GL_VERTEX_ARRAY);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void
gfx_agents_draw_new(Agent_verts* av, GLuint shader, float scale)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, av->verts_pos);
  glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, av->verts_col);
  glUseProgram(shader);

  GLuint loc = glGetUniformLocation(shader, "window");
  glUniform2f(loc, scale, scale);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glDrawArrays(GL_POINTS, 0, av->a_count);
  glDrawElements(GL_POINTS, av->a_count, GL_UNSIGNED_SHORT, (void*)0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);
  glDisableClientState(GL_VERTEX_ARRAY);

}


void
gfx_agents_draw_vis(Agent_verts* av, GLuint shader, float scale)
{

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, av->verts_vis_pos);
  glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, av->verts_vis_col);


  glUseProgram(shader);

  GLuint loc = glGetUniformLocation(shader, "window");
  glUniform2f(loc, scale, scale);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glDrawArrays(GL_POINTS, 0, av->a_count);
  glDrawElements(GL_POINTS, av->a_count, GL_UNSIGNED_SHORT, (void*)0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);
  glDisableClientState(GL_VERTEX_ARRAY);

}

/* get the scale. Since the development was done in half-screen
 * a 1.0 width is considered 1/2 of the screen. Fullscreen scaling would
 * be done by a factor of 2. Hacky, but just the way things were done*/
float
gfx_get_scale(GLFWwindow* window)
{
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w_screen = mode->width;
    int h_screen = mode->height;
    int w_window, h_window;
    float h_scale, w_scale, scale;


    glfwGetWindowSize(window, &w_window, &h_window);

    w_scale = (float)w_window / ((float)w_screen / 2.0);
    h_scale = (float)h_window / ((float)h_screen / 2.0);
    scale = (h_scale + w_scale) / 2.0f;
    return scale;
}
