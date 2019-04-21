#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "agents.h"
#include "utils.h"

typedef struct RGB_ RGB;
typedef struct _World_view World_view;
typedef struct _Framebuffer Framebuffer;

struct _World_view {
  float zoom;
  float pos_offsets[2];
};

struct _Framebuffer {
  GLuint framebuffer;
  GLuint texBuffer;
};

GLuint gfx_setup_shader(const char* vs_raw, const char* fs_raw);
GLuint gfx_agent_shader();
GLuint gfx_agent_vis_shader();

GLuint gfx_bg_shader();

Framebuffer* gfx_framebuffer_create(int width, int height);
GLuint gfx_framebuffer_shader();
void gfx_framebuffer_begin(Framebuffer* fb, World_view* wv);
void gfx_framebuffer_end();
void gfx_framebuffer_draw(Framebuffer* fb, World_view* wv, GLuint shader);

void gfx_agents_draw_cell(Agent_verts* av, GLuint shader, float scale, float zoom);
void gfx_agents_draw_vis(Agent_verts* av, GLuint shader, float scale, float zoom);


void gfx_bg_draw(GLuint shader, float time);

World_view* gfx_world_view_create();
void gfx_world_view_constrain(World_view *wv);
void gfx_world_view_zoom(World_view *wv, float xoffset, float yoffset);
void gfx_world_view_scroll(World_view *wv, float xoffset, float yoffset);
float* gfx_world_view_relpos(World_view* wv, GLFWwindow* window, float x, float y);
float gfx_get_scale(GLFWwindow* window);


#endif
