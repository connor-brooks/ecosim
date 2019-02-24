#ifndef AGENTS_H
#define AGENTS_H
#include <stdio.h>
#include "utils.h"

typedef struct _Agent {
  float x;
  float y;
  RGB rgb;
} Agent;

int agent_vert_elems(int n);
float* agents_to_vert(Agent* aa, int n);
#endif

