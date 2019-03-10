#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include "utils.h"
#include "agents.h"

#define AGENT_DIMENTIONS (4)
#define AGENT_DRAWABLE_FEATURES (2)

#define AGENT_RGB_MAX (1.0)
#define AGENT_RGB_MIN (1.0)
#define AGENT_RGB_ALPHA (0.6)

#define AGENT_MAX_VELOCITY (1.0)
#define AGENT_MIN_VELOCITY (-1.0)

#define AGENT_ENERGY_DEFAULT (1.0)
#define AGENT_METAB_MAX (0.3)

#define AGENT_METAB_ENERGY_SCALE(x) (0.005 * x)
#define AGENT_ENERGY_SIZE_SCALE(x) ((18 * x) + 2)

#define AGENT_MAX_SPEED (0.01)

#define AGENTS_ENERGY_DEAD (0.1)

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

    /* colors */
    vert_ret[c_offset++] = aa[i].rgb.r; // R
    vert_ret[c_offset++] = aa[i].rgb.g; // G
    vert_ret[c_offset++] = aa[i].rgb.b; // B
    vert_ret[c_offset++] = (aa[i].state == AGENT_STATE_PRUNE)?
      0.0f: //Pruning, hide
      AGENT_RGB_ALPHA; // normal, show

    /* pos & size */
    vert_ret[p_offset++] = aa[i].x; // x
    vert_ret[p_offset++] = aa[i].y; // y
    vert_ret[p_offset++] = 0.0f; // z
    vert_ret[p_offset++] = AGENT_ENERGY_SIZE_SCALE(aa[i].energy); //size
  }

  return vert_ret;
}

void
agents_update(Agent_array* aa)
{
  Agent* a_ptr;

  for(int i = 0; i < aa->count; i++)
  {
    /* temp pointer for agent */
    a_ptr = &aa->agents[i];

    // If the agent isn't living, don't update anything
    if(a_ptr->state != AGENT_STATE_LIVING) continue;

    agents_update_location(a_ptr);
    agents_update_energy(a_ptr);
  }
}

void
agents_update_location(Agent* a_ptr)
{
  /* Calculate move magnitude */
  float mv_amt = agents_update_mv_amt(a_ptr);

  /* Move agents */
  a_ptr->x += a_ptr->velocity.x * mv_amt;
  a_ptr->y += a_ptr->velocity.y * mv_amt;

  /* Wrap if needed */
  agents_update_mv_wrap(a_ptr);
}

float
agents_update_mv_amt(Agent* a_ptr)
{
  return AGENT_MAX_SPEED * a_ptr->metabolism;
}

void
agents_update_mv_wrap(Agent* a_ptr)
{
  float* tmp_ptr;
  /* loop through x and y and put the agents at the opposite side
   * of the screen if they're past the ends */
  for(tmp_ptr = &a_ptr->x; tmp_ptr <= &a_ptr->y; tmp_ptr++){
    *tmp_ptr = ((*tmp_ptr> WORLD_MAX_COORD) || (*tmp_ptr < WORLD_MIN_COORD)) ?
      -(*tmp_ptr) :
      *tmp_ptr;
  }
}


void
agents_update_energy(Agent* a_ptr)
{
  a_ptr->energy -= AGENT_METAB_ENERGY_SCALE(a_ptr->metabolism);
  if(a_ptr->energy < AGENTS_ENERGY_DEAD) a_ptr->state = AGENT_STATE_DEAD;
}

/* Create an agent array */
Agent_array*
agent_array_setup(int count)
{
  Agent_array* temp = malloc(sizeof(Agent_array));
  int i;

  temp->count = count;
  temp->count_change = 0;
  temp->agents = malloc(sizeof(Agent) * count);

  for(i = 0; i < count; i++)
  {
    temp->agents[i].x = RANDF_MIN(WORLD_MIN_COORD, WORLD_MAX_COORD);
    temp->agents[i].y = RANDF_MIN(WORLD_MIN_COORD, WORLD_MAX_COORD);
    // color
    temp->agents[i].rgb.r = RANDF(AGENT_RGB_MAX);
    temp->agents[i].rgb.g = RANDF(AGENT_RGB_MAX);
    temp->agents[i].rgb.b = RANDF(AGENT_RGB_MAX);
    // velocity
    temp->agents[i].velocity.x = RANDF_MIN(AGENT_MIN_VELOCITY, AGENT_MAX_VELOCITY);
    temp->agents[i].velocity.y = RANDF_MIN(AGENT_MIN_VELOCITY, AGENT_MAX_VELOCITY);
    // default state
    temp->agents[i].state = AGENT_STATE_LIVING;
    temp->agents[i].energy = AGENT_ENERGY_DEFAULT;
    temp->agents[i].metabolism = RANDF(AGENT_METAB_MAX);
  }
  return temp;
}
