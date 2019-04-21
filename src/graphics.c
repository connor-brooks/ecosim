#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics.h"
#include "agents.h"
#include "quadtree.h"
#include "utils.h"

Framebuffer*
gfx_framebuffer_create(int width, int height)
{
  Framebuffer* tmp_fb = malloc(sizeof(Framebuffer));
  GLuint framebuffer;
  GLuint texBuffer;

  /* Make framebuffer */
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  glGenTextures(1, &texBuffer);
  glBindTexture(GL_TEXTURE_2D, texBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
      GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  /* attach to framebuffer */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, texBuffer, 0);

  /* bind to default fb */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  tmp_fb->framebuffer = framebuffer;
  tmp_fb->texBuffer = texBuffer;

  return tmp_fb;
}

void
gfx_framebuffer_begin(Framebuffer* fb, World_view* wv)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer);
  glBindTexture(GL_TEXTURE_2D, fb->texBuffer);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Main drawing begin */
  glPushMatrix();
  glScalef(wv->zoom, wv->zoom, 1.);
  glTranslatef(wv->pos_offsets[0], wv->pos_offsets[1], 1.0);
}

void
gfx_framebuffer_end()
{
  glPopMatrix();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
gfx_framebuffer_draw(Framebuffer* fb, World_view* wv, GLuint shader)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
  //glViewport(0, 0, width, height);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, fb->texBuffer);

  glUseProgram(shader);
  GLuint zoom_u = glGetUniformLocation(shader, "zoom");
  glUniform1f(zoom_u, wv->zoom);
  GLuint pos_u = glGetUniformLocation(shader, "pos_offset");
  glUniform2f(pos_u, wv->pos_offsets[0], wv->pos_offsets[1]);

  /* Draw framebuffer*/
  glPushMatrix();
  glScalef(1.05, 1.05, 1.0);
  glBegin(GL_QUADS);
  glVertex2f(-1, -1);
  glVertex2f(1, -1);
  glVertex2f(1, 1);
  glVertex2f(-1, 1);
  glEnd();
  glPopMatrix();

  glUseProgram(0);
}

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
gfx_framebuffer_shader(){
  const char* test_vs =
    "#version 130\n"
    "in vec4 position;"
    "in vec4 color_in;"
    "out vec4 color_out;"
    "out vec4 pos_out;"
    "void main() {"
    "pos_out = position;"
    "color_out = color_in;"
    "gl_Position = gl_ModelViewProjectionMatrix * position;"
    "}";

  const char* test_fs =
    "#version 130\n"
    "in vec4 color_out;"
    "in vec4 pos_out;"
    "uniform sampler2D fbo_texture;"
    "uniform vec2 pos_offset;"
    "uniform float zoom;"

    "vec2 offset_tex(vec2 t_pos, vec2 wobble, float off_x, float off_y)"
    "{"
    "vec2 offset = vec2(0.0, 0.0);"
    "offset += t_pos;"
    "offset += wobble;"
    "offset += vec2(off_x, off_y);"

    "return offset;"
    "}"

    "vec2 calc_wobble(vec2 cam_offset, vec2 pos, float zoom)"
    "{"
    "vec2 wobble = vec2(0.0, 0.0);"
    "wobble.x = max(0, sin((cam_offset.y - pos.y / zoom)*16)) * 0.007;"
    "wobble.y = max(0, sin((cam_offset.x - pos.x / zoom)*24)) * 0.007;"
    "wobble *= zoom;"
    "return wobble;"
    "}"

    "void main()"
    "{"
    "vec4 t_sum = vec4(0.0);"
    "vec2 wobble;"
    "vec2 offset = vec2(0.003, 0.003);"
    "float blur_amt = 0.45 / 4;"
    "vec2 t_pos = vec2(0.5, 0.5);"
    "t_pos *= pos_out.xy;"
    "t_pos += vec2(0.5, 0.5);"

    "/* Create wobble for warping agents */"
    "wobble = calc_wobble(pos_offset.xy, pos_out.xy, zoom);"

    "/* Add offset textures, clearing wobble and blur effect */"
    "t_sum += texture2D(fbo_texture,"
    "offset_tex(t_pos, wobble, offset.x, offset.y)) * blur_amt;"
    "t_sum += texture2D(fbo_texture,"
    "offset_tex(t_pos, wobble, -offset.x, offset.y)) * blur_amt;"
    "t_sum += texture2D(fbo_texture,"
    "offset_tex(t_pos, wobble, -offset.x, -offset.y)) * blur_amt;"
    "t_sum += texture2D(fbo_texture,"
    "offset_tex(t_pos, wobble, offset.x, -offset.y)) * blur_amt;"
    "t_sum += texture2D(fbo_texture,"
    "offset_tex(t_pos, wobble, 0.0, 0.0)) * 1.0;"
    //"vec2 st = pos_offset;"
    //"st *= 1.0;"
    //"st -= 0.0;"
    "vec4 sum = vec4(0.0, 0.0, 0.0, 1.0);"
    "sum.x = wobble.x * 100;"
    "sum.z = wobble.y * 100;"
    //"sum.x = max(0, sin((st.y - pos_out.y /zoom )*16));"
    //"sum.z = max(0, sin((st.x - pos_out.x / zoom)*24));"
    " gl_FragColor = t_sum;"
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
    "gl_Position = gl_ModelViewProjectionMatrix * position;"
    "}";

  const char* world_fs =
    "#version 130\n"
    "in vec4 color_out;"
    "in vec4 pos_out;"
    "uniform float time;"

    "/* Thanks for the random function!"
    "https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83 */"
    "float rand(float n)"
    "{"
    "return fract(sin(n + fract(time)) * 43758.5453123);"
    "}"

    "void main() {"
    "vec2 pos = pos_out.xy;"
    "vec4 new_col = vec4(1.0, 1.0, 1.0, 1.0);"
    "float noise = rand(pos.x * pos.y) * 0.02;"
    "new_col.a = noise;"
    "gl_FragColor = new_col;"
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
    ""
    "void main() {"
    "color_out = color;"
    "gl_Position = gl_ModelViewProjectionMatrix *"
    "vec4(position.x, position.y, position.z, 1.0);"
    "gl_PointSize = position.w * window.x * zoom * 1.0;"
    "}";

  const char* agents_fs =
    "#version 130\n"
    "out vec4 frag_colour;"
    "in vec4 color_out;"

    "float rand(float n)"
    "{"
    "return fract(sin(n) * 43758.5453123);"
    "}"

    "void main() {"
    " vec4 color = color_out;"
    " vec2 pos = gl_PointCoord - vec2(0.5);"
    " float pat_r = length(pos)*1.0;"
    " float pat_a = atan(pos.y,pos.x);"
    " float pat_f = cos(pat_a * 80);"
    " float alpha =  smoothstep(pat_f,pat_f+0.02,pat_r) * 1.0;"
    " float cut_r = 1.0;"
    " float cutoff = 1.0 - smoothstep(cut_r - (cut_r * 0.02),"
    " cut_r + (cut_r * 0.02),"
    " dot(pos, pos) * 4.0);"
    " if(color_out.a == 0) color.a = 0;"
    " gl_FragColor =  alpha * cutoff * color "
    "* ((1.0 - (length(pos) )) * 1.5) ;"
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
    ""
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
float*
gfx_world_view_relpos(World_view* wv, GLFWwindow* window, float x, float y)
{
  float xRel, yRel;
  int win_width, win_height;
  float* tmp = malloc(sizeof(float) * 2);
  glfwGetWindowSize(window, &win_width, &win_height);

  xRel = x / (float) win_width;
  yRel = y / (float) win_height;
  xRel *= 2;
  yRel *= 2;
  xRel -= 1;
  yRel -= 1;
  yRel = -yRel;
  xRel = (xRel - wv->pos_offsets[0] * wv->zoom) / wv->zoom;
  yRel = (yRel - wv->pos_offsets[1]* wv->zoom) / wv->zoom;

  tmp[0] = xRel;
  tmp[1] = yRel;

  return tmp;
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
