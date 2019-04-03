#ifndef AGENTS_H
#define AGENTS_H
#include <stdio.h>
#include <stddef.h>
#include "utils.h"
#include "quadtree.h"

#define AGENT_DIMENTIONS (4)
#define AGENT_DRAWABLE_FEATURES (2)

#define AGENT_RGB_MAX (1.0)
#define AGENT_RGB_MIN (1.0)
#define AGENT_RGB_ALPHA (0.8)

#define AGENT_VIS_ALPHA (0.08)

#define AGENT_MAX_VELOCITY (1.0)
#define AGENT_MIN_VELOCITY (-1.0)

#define AGENT_ENERGY_DEFAULT (1.0)
#define AGENT_METAB_MAX (0.5)
#define AGENT_METAB_MIN (0.0)

#define AGENT_FEAR_MAX (1.0)
#define AGENT_FEAR_MIN (-1.0)

#define AGENT_VISION_MAX (0.05)
#define AGENT_VISION_MIN (0.25)

#define AGENT_METAB_ENERGY_SCALE(x) (0.001 * x)
#define AGENT_ENERGY_SIZE_SCALE(x) ((20 * x) + 2)

#define AGENT_MAX_SPEED (0.005)

#define AGENTS_ENERGY_DEAD (0.1)

#define AGENTS_TIME_FACTOR (0.5)

#define AGENT_ARRAY_DEFAULT_SIZE (16)
#define AGENT_DEBUG_SHOW_VISION (0)

#define AGENT_VIS_VERTS_DEFAULT (16)

typedef struct _Agent Agent;
typedef struct _Agent_array Agent_array;
typedef struct _Agent_verts Agent_verts;
typedef struct _Agent_vis_verts Agent_vis_verts;
typedef struct _DNA DNA;

enum agent_states {
  AGENT_STATE_PRUNE  = -1,
  AGENT_STATE_DEAD   =  0,
  AGENT_STATE_LIVING =  1

};

struct _DNA {
  float metabolism;
  float fear;
  float vision;

};

struct _Agent {
  float x;
  float y;

  RGB rgb;

  struct {
    float x;
    float y;
  } velocity;

  int state;
  float energy;
  DNA dna;

  struct {
    float bot_left[2];
    float top_right[2];
  } vis_quad;


};

struct _Agent_array {
  Agent** agents;
  size_t count;
  size_t capacity;
  size_t size;
  int count_change;
};


struct _Agent_verts {
  float* verts_pos, *verts_col;
  float* verts_vis_pos, *verts_vis_col;
  size_t capacity;
  size_t size;
  int a_count;
  ptrdiff_t end;
};

struct _Agent_vis_verts {
  float* pos_verts, *col_verts;
  size_t count;
  size_t capacity;
  size_t pos_size, col_size;
};


Agent_array* agent_array_create();
Agent_array* agent_array_setup_random(int count);
void agent_array_insert(Agent_array* aa, Agent* a);
void agent_array_free(Agent_array* aa);
void agents_update(Agent_array* aa, Quadtree* quad);


Agent_array* agents_get_local(Agent* a_ptr, Quadtree* quad, float radius);

Agent* agent_create_random();
void agent_setup_colors(Agent* a_ptr);
void agent_setup_vision_quad(Agent* a_ptr);
void agents_update_location(Agent* a_ptr);
void agents_update_energy(Agent* a_ptr);
float agents_update_mv_amt(Agent* a_ptr);
void agents_update_mv_wrap(Agent* a_ptr);

void agent_normalize_velocity(Agent* a_ptr);
void agent_update_mv_avoid(Agent* a_ptr, Agent* t_ptr);
float agent_item_attraction(Agent* a_ptr, Agent* t_ptr);

void agent_update_mv_flock(Agent* a_ptr, Agent_array* aa);

/* verts */
Agent_verts* agent_verts_create();
void agent_verts_free(Agent_verts* av);
void agents_to_verts(Agent_array* aa, Agent_verts* av);

Agent_vis_verts* agent_vis_verts_create();

#endif

