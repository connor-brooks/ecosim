#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "utils.h"
#include "agents.h"
#define AGENT_DIMENTIONS (4)
#define AGENT_DRAWABLE_FEATURES (2)

int 
agent_vert_elems(int n)
{
  return AGENT_DRAWABLE_FEATURES *
    AGENT_DIMENTIONS * n;
}


float*
agents_to_vert(Agent* aa, int n) 
{
  int i;
  ptrdiff_t p_offset, c_offset;
  size_t verts_len = agent_vert_elems(n) * sizeof(float);
  float* verts = malloc(verts_len);

  for(i = 0; i < n; i++) {
    p_offset = i * AGENT_DIMENTIONS;
    c_offset = p_offset + (n * AGENT_DIMENTIONS);

    verts[p_offset++] = aa[i].x; // x
    verts[p_offset++] = aa[i].y; // y
    verts[p_offset++] = 0.0f; // z 
    verts[p_offset++] = 1.0f;

    verts[c_offset++] = 1; // R
    verts[c_offset++] = 2; // G
    verts[c_offset++] = 3; // B
    verts[c_offset++] = 4; // A
  }
  return verts;
}
