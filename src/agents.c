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
agents_to_vert(Agent* aa, int n, float* vert_arr, int mode)
{
  int i;
  ptrdiff_t p_offset, c_offset;
  size_t verts_len = agent_vert_elems(n) * sizeof(float);
  float* vert_ret;

  vert_ret  = (mode) ? vert_arr : malloc(verts_len);

  for(i = 0; i < n; i++) {
    p_offset = i * AGENT_DIMENTIONS;
    c_offset = p_offset + (n * AGENT_DIMENTIONS);

    switch(mode){
      case VERTS_NEW:
        vert_ret[c_offset++] = aa[i].rgb.r; // R
        vert_ret[c_offset++] = aa[i].rgb.g; // G
        vert_ret[c_offset++] = aa[i].rgb.b; // B
        vert_ret[c_offset++] = 1.0f; // A
      case VERTS_UPDATE:
        vert_ret[p_offset++] = aa[i].x; // x
        vert_ret[p_offset++] = aa[i].y; // y
        vert_ret[p_offset++] = 0.0f; // z
        vert_ret[p_offset++] = 1.0f;
        break;

    };
  }
  return vert_ret;
}
