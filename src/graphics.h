#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "agents.h"
#include "quadtree.h"

typedef struct RGB_ RGB;

struct _Ui_graphics {
  float height;
  float* vertex_data;
  RGB* color;
  char* cmd_txt;
  int show_sel;
  float selection[4];
};

typedef struct _Ui_graphics Ui_graphics;

GLuint gfx_setup_shader(const char* vs_raw, const char* fs_raw);
GLuint gfx_agent_shader();
GLuint gfx_agent_vis_shader();

GLuint gfx_world_shader();

void gfx_agents_draw_new(Agent_verts* av, GLuint shader, float scale, float zoom);
void gfx_agents_draw_vis(Agent_verts* av, GLuint shader, float scale, float zoom);

void gfx_quad_draw(Quadtree_verts* qv);

void gfx_text_draw(float x, float y, const unsigned char* txt);

float gfx_get_scale(GLFWwindow* window);

void gfx_world_texture(GLuint shader, float time);


#endif
