#ifndef AGENTS_H
#define AGENTS_H
#include <stdio.h>
#include <stddef.h>
#include "utils.h"

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


enum agent_to_verts_mode {
  VERTS_NEW,
  VERTS_UPDATE
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

int agent_vert_elems(int n);
float* agents_to_vert(Agent* aa, int n, float* vert_arr, int mode);

void agents_to_verts(Agent_array* aa, Agent_verts* av);
Agent_verts* agent_verts_create();

#endif

