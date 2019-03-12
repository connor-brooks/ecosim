#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <GL/glew.h>
#include "agents.h"
#include "quadtree.h"
#include "ui_response.h"

typedef struct RGB_ RGB;

struct _Agent_graphics {
  GLuint vbo;
  GLuint shader;
  int no_agents;
  float* vert_data;
  size_t vert_data_len;
  const char* vert_shader;
  const char* frag_shader;
};

struct _Ui_graphics {
  float height;
  float* vertex_data;
  RGB* color;
  char* cmd_txt;
  int show_sel;
  float selection[4];
};

typedef struct _Agent_graphics Agent_graphics;
typedef struct _Ui_graphics Ui_graphics;

Agent_graphics* agent_gfx_setup(int count, float* verts, 
    const char* v_shdaer, const char* f_shader);
void agent_vbo_setup(Agent_graphics* ag);
void agent_vbo_update(Agent_graphics* ag, Agent_array* aa);
void agent_shader_setup(Agent_graphics* ag);
void gfx_agents_draw(Agent_graphics* ag);

void gfx_agents_draw_new(Agent_verts* av, GLuint shader);

void gfx_quad_draw(Quadtree_verts* qv);

Ui_graphics* ui_gfx_setup(void);
void ui_draw(Ui_graphics* uig);
void ui_gfx_update(Ui_resp* resp, Ui_graphics* uig);

void draw_text(float x, float y, const unsigned char* txt);
#endif
