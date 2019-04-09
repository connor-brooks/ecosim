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
#define AGENT_RGB_ALPHA (0.9)

#define AGENT_VIS_ALPHA (0.2)

#define AGENT_MAX_VELOCITY (1.0)
#define AGENT_MIN_VELOCITY (-1.0)

#define AGENT_ENERGY_DEFAULT (1.0)
#define AGENT_METAB_MAX (0.5)
#define AGENT_METAB_MIN (0.05)

#define AGENT_FEAR_MAX (1.0)
#define AGENT_FEAR_MIN (-1.0)

#define AGENT_VISION_MAX (0.1)
#define AGENT_VISION_MIN (0.175)

#define AGENT_REBIRTH_MAX (3.00)
#define AGENT_REBIRTH_MIN (1.00)

#define AGENT_AGGRESION_MAX (1.00)
#define AGENT_AGGRESION_MIN (-1.00)

#define AGENT_DIET_MAX (1.00)
#define AGENT_DIET_MIN (-1.00)

#define AGENT_FLOCK_MAX (1.00)
#define AGENT_FLOCK_MIN (0.00)

#define AGENT_WOBBLE_MAX (15.00)
#define AGENT_WOBBLE_MIN (3.00)

#define AGENT_METAB_ENERGY_SCALE(x) (0.0015 * x)
#define AGENT_ENERGY_SIZE_SCALE(x) ((10 * x) + 2)

#define AGENT_MAX_SPEED (0.0015)

#define AGENTS_ENERGY_DEAD (0.3)

#define AGENTS_TIME_FACTOR (0.5)

//#define AGENTS_DNA_MUTATE_RATE (0.05)
#define AGENTS_DNA_MUTATE_RATE (0.075)

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

enum agent_diet {
  AGENT_DIET_LIVING,
  AGENT_DIET_DEAD
};

struct _DNA {
  float metabolism;
  float fear;
  float vision;
  float rebirth;
  float aggresion;
  float diet; /* -1 for dead, 1 for living */
  float flock;
  float wobble;

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
Agent_array* agent_array_prune(Agent_array* aa);
void agents_update(Agent_array* aa, Quadtree* quad);


Agent_array* agents_get_local(Agent* a_ptr, Quadtree* quad, float radius);

Agent* agent_create_random();
void agent_setup_colors(Agent* a_ptr);
void agent_setup_vision_quad(Agent* a_ptr);
void agents_update_location(Agent* a_ptr);
void agents_update_energy(Agent* a_ptr);
float agents_update_mv_amt(Agent* a_ptr);
void agents_update_mv_wrap(Agent* a_ptr);
void agents_insert_dead(Agent_array* aa, int count);

void agent_normalize_velocity(Agent* a_ptr);
void agent_update_mv_avoid(Agent* a_ptr, Agent* t_ptr);
float agent_item_attraction(Agent* a_ptr, Agent* t_ptr, float* mag);

void agent_item_collision(Agent* a_ptr, Agent* t_ptr);

void agent_split(Agent* a_ptr, Agent_array* aa);
void agent_dna_mutate(Agent* a_ptr);

void agent_update_mv_flock(Agent* a_ptr, Agent_array* aa);

/* verts */
Agent_verts* agent_verts_create();
void agent_verts_free(Agent_verts* av);
void agents_to_verts(Agent_array* aa, Agent_verts* av);

Agent_vis_verts* agent_vis_verts_create();

#endif

