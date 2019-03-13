#ifndef AGENTS_H
#define AGENTS_H
#include <stdio.h>
#include <stddef.h>
#include "utils.h"

#define AGENT_DIMENTIONS (4)
#define AGENT_DRAWABLE_FEATURES (2)

#define AGENT_RGB_MAX (1.0)
#define AGENT_RGB_MIN (1.0)
#define AGENT_RGB_ALPHA (0.6)

#define AGENT_MAX_VELOCITY (1.0)
#define AGENT_MIN_VELOCITY (-1.0)

#define AGENT_ENERGY_DEFAULT (1.0)
#define AGENT_METAB_MAX (0.6)
#define AGENT_METAB_MIN (0.0)

#define AGENT_METAB_ENERGY_SCALE(x) (0.005 * x)
#define AGENT_ENERGY_SIZE_SCALE(x) ((4 * x) + 2)

#define AGENT_MAX_SPEED (0.01)

#define AGENTS_ENERGY_DEAD (0.1)

#define AGENTS_TIME_FACTOR (0.5)

typedef struct _Agent Agent;
typedef struct _Agent_array Agent_array;
typedef struct _Agent_verts Agent_verts;

enum agent_states {
  AGENT_STATE_PRUNE  = -1,
  AGENT_STATE_DEAD   =  0,
  AGENT_STATE_LIVING =  1
 
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
  float metabolism;

};

struct _Agent_array {
  Agent* agents;
  size_t count;
  int count_change;
};


struct _Agent_verts {
  float* verts_pos, *verts_col;
  size_t capacity;
  size_t size;
  int a_count;
  ptrdiff_t end;
};

void agents_update(Agent_array* aa);

void agents_update_location(Agent* a_ptr);

void agents_update_energy(Agent* a_ptr);

float agents_update_mv_amt(Agent* a_ptr);

void agents_update_mv_wrap(Agent* a_ptr);

Agent_array* agent_array_setup(int count);

void agents_to_verts(Agent_array* aa, Agent_verts* av);
Agent_verts* agent_verts_create();
void agent_verts_free(Agent_verts* av);

#endif

