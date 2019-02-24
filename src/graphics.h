#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <GL/glew.h>

typedef struct Rectangle_ Rectangle;
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

typedef struct _Agent_graphics Agent_graphics;

void agent_vbo_setup(Agent_graphics* ag);
void agent_shader_setup(Agent_graphics* ag);
void agents_draw(Agent_graphics* ag);
void draw_rectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void draw_text(float x, float y, const unsigned char* txt);
void draw_point(float x, float y, float r, float g, float b);
void draw_rectangle_struct(Rectangle* rect, RGB* c);
#endif
