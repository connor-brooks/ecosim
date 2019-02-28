#ifndef AGENTS_H
#define AGENTS_H
#include <stdio.h>
#include "utils.h"

typedef struct _Agent Agent;
typedef struct _Agent_array Agent_array;

struct _Agent {
  float x;
  float y;
  float size;
  RGB rgb;
};

struct _Agent_array {
  Agent* agents;
  int count;
};


enum agent_to_verts_mode {
  VERTS_NEW,
  VERTS_UPDATE
};

void agents_update(Agent_array* aa);
Agent_array* agent_array_setup(int count);

int agent_vert_elems(int n);
float* agents_to_vert(Agent* aa, int n, float* vert_arr, int mode);

#endif

