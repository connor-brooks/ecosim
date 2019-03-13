#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include "utils.h"
#include "agents.h"


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
    temp->agents[i].rgb.b = temp->agents[i].rgb.g; //RANDF(AGENT_RGB_MAX);
    // velocity
    temp->agents[i].velocity.x = RANDF_MIN(AGENT_MIN_VELOCITY, AGENT_MAX_VELOCITY);
    temp->agents[i].velocity.y = RANDF_MIN(AGENT_MIN_VELOCITY, AGENT_MAX_VELOCITY);
    // default state
    temp->agents[i].state = AGENT_STATE_LIVING;
    temp->agents[i].energy = AGENT_ENERGY_DEFAULT;
    temp->agents[i].metabolism = RANDF_MIN(AGENT_METAB_MIN, AGENT_METAB_MAX);
  }
  return temp;
}
void 
agent_array_free(Agent_array* aa)
{
  free(aa->agents);
  free(aa);
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
    /* experiemtation code.. ignore  */
    float mv_amt = agents_update_mv_amt(a_ptr);
    a_ptr->x += mv_amt * 0.2 * a_ptr->energy * sin(a_ptr->metabolism + glfwGetTime() * (2 * a_ptr->energy));
    a_ptr->y += mv_amt * 0.2 * a_ptr->energy * sin(a_ptr->metabolism + glfwGetTime() * (2 * a_ptr->energy));
    /*    */
    agents_update_energy(a_ptr);
  }
}

void
agents_update_location(Agent* a_ptr)
{
  /* Calculate move magnitude */
  float mv_amt = agents_update_mv_amt(a_ptr);

  /* also change velocity into an array */
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

/* This function is stupid.
 * Put X and y for agents into an arrary so this type
 * of thing doesn't exist. */
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
  a_ptr->energy -= AGENT_METAB_ENERGY_SCALE(a_ptr->metabolism) * AGENTS_TIME_FACTOR;
  if(a_ptr->energy < AGENTS_ENERGY_DEAD) a_ptr->state = AGENT_STATE_DEAD;
}

Agent_verts*
agent_verts_create()
{
  Agent_verts* tmp = malloc(sizeof(Agent_verts));
  tmp->capacity = sizeof(float) * (4);
  tmp->size = 0;
  tmp->verts_pos = malloc(tmp->capacity);
  tmp->verts_col = malloc(tmp->capacity);
  tmp->a_count = 0;
  tmp->end = 0;
  return tmp;
  //
}
void 
agent_verts_free(Agent_verts* av)
{
  free(av->verts_pos);
  free(av->verts_col);
  free(av);
}

void
agents_to_verts(Agent_array* aa, Agent_verts* av)
{
  int i;
  size_t new_size = (sizeof(float) * 4 * aa->count); //4 floats
  av->end = 0;
  av->a_count= 0;

  //  if(aa->count_change) printf("rebuuld aarrary\n");

  /* if verts array too big, grow */
  if(new_size > av->capacity){
    av->capacity = new_size;
    av->verts_pos = realloc(av->verts_pos, av->capacity);
    av->verts_col = realloc(av->verts_col, av->capacity);
  }


  for(i = 0; i < aa->count ; i++) {
    Agent* agent = &aa->agents[i];
    float* pos = av->verts_pos;
    float* col = av->verts_col;
    //printf("adding agent %f %f\n", agent->x, agent->y);

    pos[av->end] = agent->x;
    av->verts_col[av->end] = agent->rgb.r;
    av->end++;

    pos[av->end] = agent->y;
    col[av->end] = agent->rgb.g;
    av->end++;

    pos[av->end] = 0.0f;
    col[av->end] = agent->rgb.b;
    av->end++;

    pos[av->end] = AGENT_ENERGY_SIZE_SCALE(agent->energy);
    col[av->end] = (agent->state == AGENT_STATE_PRUNE)?
      0.0f : // pruning
      AGENT_RGB_ALPHA;
    av->end++;
    //printf("At %d\n", i);
    av->a_count++;

  }

}
