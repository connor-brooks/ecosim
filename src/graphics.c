#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics.h"
#include "agents.h"
#include "quadtree.h"
#include "utils.h"

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
gfx_test_shader(){
  const char* test_vs =
    "#version 130\n"
    "in vec4 position;"
    "in vec4 color_in;"
    "out vec4 color_out;"
    "out vec4 pos_out;"
    "void main() {"
    "pos_out = position;"
    "color_out = color_in;"
    "  gl_Position =   position;"
    "}";

  const char* test_fs =
    "#version 130\n"
    "in vec4 color_out;"
    "in vec4 pos_out;"
    "uniform sampler2D fbo_texture;"
    "uniform vec2 pos_offset;"
    "uniform float zoom;"
    "uniform float time;"
    "void main() {"
    "vec2 textpos = vec2(0.5, 0.5);"
    "vec2 offset = vec2(0.003, 0.003);"
    "textpos *= pos_out.xy;"
    "textpos += vec2(0.5, 0.5);"
    "vec4 sum = vec4(0.0);"
    //"float test = max(0, sin((pos_offset.y - pos_out.y)*16 / zoom)) * 0.007;"
    //"float test_two = max(0, sin((pos_offset.x - pos_out.x)*16 / zoom)) * 0.007;"
    //

    "float test = max(0, sin((pos_offset.y - pos_out.y / zoom)*16)) * 0.007;"
    "float test_two = max(0, sin((pos_offset.x - pos_out.x / zoom)*24)) * 0.007;"

    "sum = texture2D(fbo_texture, textpos + vec2(test, test_two )) ;"

    "vec2 wobble = vec2(test, test_two) * zoom;"
    "float blur_amt = 0.4 / 4;"
    "sum = vec4(0);"
    "sum += texture2D(fbo_texture, textpos + wobble +vec2(offset.x, offset.y)) * blur_amt;"
    "sum += texture2D(fbo_texture, textpos + wobble +vec2(-offset.x, offset.y)) * blur_amt;"
    "sum += texture2D(fbo_texture, textpos + wobble +vec2(-offset.x, -offset.y)) * blur_amt;"
    "sum += texture2D(fbo_texture, textpos + wobble +vec2(offset.x, -offset.y)) * blur_amt;"
    "sum += texture2D(fbo_texture, textpos + wobble) * 1.0;"
    "vec2 st = pos_offset;"
    "st *= 1.0;"
    "st -= 0.0;"

    //"sum.x = max(0, sin((st.y - pos_out.y /zoom )*16));"
    //"sum.z = max(0, sin((st.x - pos_out.x / zoom)*24));"
    " gl_FragColor = sum;"
    "}";

  return gfx_setup_shader(test_vs, test_fs);

}

GLuint
gfx_world_shader()
{
  const char* world_vs =
    "#version 130\n"
    "in vec4 position;"
    "in vec4 color_in;"
    "uniform float time;"
    "out vec4 color_out;"
    "out vec4 pos_out;"
    "void main() {"
    "pos_out = position;"
    "color_out = color_in;"
    "  gl_Position = position;"
    "}";

  const char* world_fs =
    "#version 130\n"
    "in vec4 color_out;"
    "in vec4 pos_out;"
    "uniform float time;"
    "float rand(float n){return fract(sin(n + fract(time)) * 43758.5453123);}"
    "void main() {"
    " vec2 pos = pos_out.xy;"
    "vec4 new = vec4(1.0, 1.0, 1.0, 1.0);"
    "float noise = rand(pos.x * pos.y) * 0.02;"
    "new.w = noise;"
    "  gl_FragColor = new;"
    "}";

  return gfx_setup_shader(world_vs, world_fs);
}

GLuint
gfx_agent_shader()
{
  const char* agents_vs =
    "#version 130\n"
    "attribute vec4 position;"
    "attribute vec4 color;"
    "uniform vec2 window;"
    "uniform float zoom;"
    "out vec4 color_out;"
    "void main() {"
    "color_out = color;"
    "  gl_Position = gl_ModelViewProjectionMatrix  * vec4(position.x, position.y, position.z, 1.0);"
    "  gl_PointSize = position.w * window.x * zoom * 1.0;"
    "}";

  const char* agents_fs =
    "#version 130\n"
    "out vec4 frag_colour;"
    "in vec4 color_out;"
    "float rand(float n){return fract(sin(n) * 43758.5453123);}"
    " "
    "void main() {"
    " vec4 color = color_out;"
    " vec2 pos = gl_PointCoord - vec2(0.5);"
    " vec2 diff_pos = pos;"
    " diff_pos.x *= 1.0;"
    " diff_pos.y *= 1.0;"
    " float r = length(diff_pos)*1.0;"
    " float a = atan(pos.y,pos.x);"
    " float f = cos(a * 80);"
    " float alpha =  smoothstep(f,f+0.02,r) * 1.0;"
    " float radius = 1.0;" //0.25;;"
    " float cutoff = 1.0 - smoothstep(radius - (radius * 0.01),"
    " radius+(radius * 0.01),"
    " dot(diff_pos, diff_pos) * 4.0);"
    //" if(length(pos) > 0.5) discard;"
    //" color.w = (alpha + color.w) ;"
    " if(color_out.w == 0) color.w = 0;"// 1.5 made it brighter
    " gl_FragColor =  alpha * cutoff * color * ((1.0 - (length(pos) )) * 1.5) ;"
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
    "uniform float zoom;"
    "void main() {"
    "color_out = color;"
    "  gl_Position = gl_ModelViewProjectionMatrix * vec4(position.xyz, 1.0);"
    "  gl_PointSize = position.w * window.x * zoom;"
    "}";

  const char* vis_fs =
    "#version 130\n"
    "out vec4 frag_colour;"
    "in vec4 color_out;"
    "uniform float zoom;"
    "float rand(float n){return fract(sin(n) * 43758.5453123);}"
    "float mini_rand(float n){return fract(sin(n) * 150);}"
    "void main() {"
    "vec4 color = color_out;"
    " vec2 pos = gl_PointCoord - vec2(0.5);"
    "float noise = rand(pos.x * pos.y) * 0.07;"
    "color.w = color.w + noise;"

    " float radius = 1.0;" //0.25;;"
    " float cutoff = 1.0 - smoothstep(radius - (radius * 0.01),"
    " radius+(radius * 0.01),"
    " dot(pos, pos) * 4.0);"

    " float noise_pat = mini_rand((pos.x + 0.5) * (pos.y + 0.5));"
    "color += vec4(noise_pat * 0.15);"
    "  gl_FragColor = cutoff * color * fract((length(pos) - 0.4) * 4.0) + 0.02;"
    " if(color_out.w == 0) gl_FragColor.w = 0;"
    "}";

  return gfx_setup_shader(vis_vs, vis_fs);
}

void
gfx_quad_draw(Quadtree_verts* qv)
{
  glColor4f(1.0, 1.0, 1.0, 0.02);
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, qv->verts);
  glDrawArrays(GL_QUADS, 0, qv->q_count * 4);
  glDisableClientState(GL_VERTEX_ARRAY);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void
gfx_agents_draw_new(Agent_verts* av, GLuint shader, float scale, float zoom)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, av->verts_pos);
  glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, av->verts_col);
  glUseProgram(shader);

  GLuint loc = glGetUniformLocation(shader, "window");
  glUniform2f(loc, scale, scale);

  GLuint loc2 = glGetUniformLocation(shader, "zoom");
  glUniform1f(loc2, zoom);

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
gfx_agents_draw_vis(Agent_verts* av, GLuint shader, float scale, float zoom)
{

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, av->verts_vis_pos);
  glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, av->verts_vis_col);

  glUseProgram(shader);

  GLuint loc = glGetUniformLocation(shader, "window");
  glUniform2f(loc, scale, scale);

  GLuint loc2 = glGetUniformLocation(shader, "zoom");
  glUniform1f(loc2, zoom);

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

World_view*
gfx_world_view_create()
{
  World_view* tmp = malloc(sizeof(World_view));
  tmp->zoom = 1.0f;
  tmp->pos_offsets[0] = 0.0f;
  tmp->pos_offsets[1] = 0.0f;
  return tmp;
}

void
gfx_world_view_zoom(World_view *wv, float xoffset, float yoffset)
{
  xoffset *= 0.020;
  yoffset *= 0.020;

  wv->zoom += yoffset;

  wv->zoom = MAX(1.0, wv->zoom);
  wv->zoom = MIN(2.0, wv->zoom);

  /* keep in frame */
  gfx_world_view_constrain(wv);

}

void
gfx_world_view_scroll(World_view *wv, float xoffset, float yoffset)
{
  xoffset *= 0.020;
  yoffset *= 0.020;

  wv->pos_offsets[0] += xoffset;
  wv->pos_offsets[1] += -yoffset;

  gfx_world_view_constrain(wv);

}

void
gfx_world_view_constrain(World_view *wv)
{
  float max;
  max = (1 / wv->zoom) - 1;
  wv->pos_offsets[0] = MAX(max, wv->pos_offsets[0]);
  wv->pos_offsets[0] = MIN(-max, wv->pos_offsets[0]);
  wv->pos_offsets[1] = MAX(max, wv->pos_offsets[1]);
  wv->pos_offsets[1] = MIN(-max, wv->pos_offsets[1]);
}

void gfx_world_texture(GLuint shader, float time)
{
  glColor3f(0.0, 1.0, 0.0);
  glUseProgram(shader);

  GLuint loc = glGetUniformLocation(shader, "time");
  glUniform1f(loc, time);

  glBegin(GL_QUADS);
  glVertex3f(-1.0f, -1.0f, 0.0f);
  glVertex3f(1.0f, -1.0f, 0.0f);
  glVertex3f(1.0f, 1.0f, 0.0f);
  glVertex3f(-1.0f, 1.0f, 0.0f);
  glEnd();
  glUseProgram(0);
}
