#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include "utils.h"
#include "agents.h"
#include "quadtree.h"


/* Create empty agent array */
Agent_array*
agent_array_create()
{
  Agent_array* temp = malloc(sizeof(Agent_array));
  temp->count = 0;
  temp->capacity = AGENT_ARRAY_DEFAULT_SIZE;
  temp->size = sizeof(Agent*) * temp->capacity;
  temp->agents = malloc(temp->size);

  temp->count_change = 0;

  return temp;
}

/* To insert agent to agent array */
void
agent_array_insert(Agent_array* aa, Agent* a)
{
  /* resize of needed */
  if(aa->count + 1 >= aa->capacity){
    aa->capacity *= 2;
    aa->size = sizeof(Agent*) * aa->capacity;
    aa->agents = realloc(aa->agents, aa->size);
  }
  /* save the agent to array */
  aa->agents[aa->count] = a;
  aa->count++;
}

/* Create random agent */
Agent*
agent_create_random()
{
  Agent* tmp_agent = malloc(sizeof(Agent));
  /* random DNA */
  tmp_agent->dna.metabolism = RANDF_MIN(AGENT_METAB_MIN, AGENT_METAB_MAX);
  tmp_agent->dna.fear = RANDF_MIN(AGENT_FEAR_MIN, AGENT_FEAR_MAX);
  tmp_agent->dna.vision = RANDF_MIN(AGENT_VISION_MIN, AGENT_VISION_MAX);

  /* random pos / directional info */
  tmp_agent->x = RANDF_MIN(WORLD_MIN_COORD, WORLD_MAX_COORD);
  tmp_agent->y = RANDF_MIN(WORLD_MIN_COORD, WORLD_MAX_COORD);
  tmp_agent->velocity.x = RANDF_MIN(AGENT_MIN_VELOCITY, AGENT_MAX_VELOCITY);
  tmp_agent->velocity.y = RANDF_MIN(AGENT_MIN_VELOCITY, AGENT_MAX_VELOCITY);



  /* set colors based on DNA */
  agent_setup_colors(tmp_agent);
  agent_setup_vision_quad(tmp_agent);

  // default state
  tmp_agent->state = AGENT_STATE_LIVING;
  tmp_agent->energy = AGENT_ENERGY_DEFAULT;

  return tmp_agent;

}

/* rules for colors */
void
agent_setup_colors(Agent* a_ptr)
{
  a_ptr->rgb.r = (a_ptr->dna.fear > 0)? 0.0f : 1.0f;
  a_ptr->rgb.g = a_ptr->dna.metabolism * 2.0;
  a_ptr->rgb.b = (a_ptr->dna.fear < 0)? 0.0f : 1.0f;
}

void
agent_setup_vision_quad(Agent* a_ptr)
{
  float half_rad = a_ptr->dna.vision * 0.5;

  /* radius sized box around agent */
  a_ptr->vis_quad.bot_left[0] = a_ptr->x - half_rad;
  a_ptr->vis_quad.bot_left[1] = a_ptr->y - half_rad;

  a_ptr->vis_quad.top_right[0] = a_ptr->x + half_rad;
  a_ptr->vis_quad.top_right[1] = a_ptr->y + half_rad;
}

/* For testing: Create an agent array with randomised population*/
Agent_array*
agent_array_setup_random(int count)
{
  int i;
  Agent_array* tmp_aa = agent_array_create();
  Agent* tmp_a = agent_create_random();

  for(i = 0; i < count; i++)
  {
    tmp_a = agent_create_random();
    agent_array_insert(tmp_aa, tmp_a);
  }
  return tmp_aa;
}

void
agent_array_free(Agent_array* aa)
{
  int i;
  /* free each agent */
  for(i = 0; i < aa->count; i++)
    free(aa->agents[i]);

  /* free everything else */
  free(aa->agents);
  free(aa);
}

/* update agents from agent array */
void
agents_update(Agent_array* aa, Quadtree* quad)
{
  int i;
  Agent* a_ptr;
  Agent_array* local_agents;

  for(i = 0; i < aa->count; i++)
  {
    /* temp pointer for agent */
    a_ptr = aa->agents[i];

    // If the agent isn't living, don't update anything
    if(a_ptr->state != AGENT_STATE_LIVING) continue;

    /* get local agents */
    local_agents = agents_get_local(a_ptr, quad, a_ptr->dna.vision);
    for(int j = 0; j < local_agents->count; j++){
      agent_update_mv_avoid(a_ptr, local_agents->agents[j]);
    }
    agent_update_mv_flock(a_ptr, local_agents);

    /* updates */
    agents_update_location(a_ptr);

    agents_update_energy(a_ptr);


    /* radius sized box around agent */
    agent_setup_vision_quad(a_ptr);
    float *bot_left  = (a_ptr->vis_quad.bot_left);
    float *top_right = (a_ptr->vis_quad.top_right);
    /* debug */
    if(AGENT_DEBUG_SHOW_VISION){
      //quadtree_query_dump(query);
      glColor4f(a_ptr->rgb.r, a_ptr->rgb.g, a_ptr->rgb.b, 0.03);
      glBegin(GL_QUADS);
      glVertex3f(bot_left[0], bot_left[1], 0.0);
      glVertex3f(top_right[0], bot_left[1], 0.0);
      glVertex3f(top_right[0], top_right[1], 0.0);
      glVertex3f(bot_left[0], top_right[1], 0.0);

      glEnd();

    }

  }
}

Agent_array*
agents_get_local(Agent* a_ptr, Quadtree* quad, float radius)
{
  int i;
  int ignore;

  Quadtree_query* query = quadtree_query_setup();
  Agent_array* agent_array = agent_array_create();
  Agent* tmp_a;

  float pos[] = {a_ptr->x, a_ptr->y};
  float *bot_left  = (a_ptr->vis_quad.bot_left);
  float *top_right = (a_ptr->vis_quad.top_right);


  /* Do query */
  quadtree_query(quad, query, pos, radius);

  /* Loop through agents */
  for(i = 0; i < query->ptr_count; i++)
  {
    tmp_a = query->ptrs[i];
    ignore = 0;

    /* Ignore null pointers and self */
    if(tmp_a == NULL)
      ignore = 1;
    if(a_ptr == tmp_a)
      ignore = 1;

    /* Ignore not within Agents line of sight? */
    if(tmp_a->x > top_right[0] ||
        tmp_a->x < bot_left[0]  ||
        tmp_a->y > top_right[1] ||
        tmp_a->y < bot_left[1])
      ignore = 1;

    /* Ignore pruneable agents */
    if(tmp_a->state == AGENT_STATE_PRUNE)
      ignore = 1;

    /* Insert if valid agent */
    if(!ignore) agent_array_insert(agent_array, tmp_a);
  }

  quadtree_query_free(query);
  return agent_array;
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
  return AGENT_MAX_SPEED * a_ptr->dna.metabolism;
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

float
agent_item_attraction(Agent* a_ptr, Agent* t_ptr)
{
  float attraction = a_ptr->dna.fear;
  if(a_ptr->dna.fear >= 0.0 && t_ptr->state == AGENT_STATE_DEAD)
    attraction = -attraction;
  return attraction;
}

void
agent_normalize_velocity(Agent* a_ptr)
{
  float mag = sqrt((a_ptr->velocity.x * a_ptr->velocity.x)
      + (a_ptr->velocity.y * a_ptr->velocity.y));
  a_ptr->velocity.x = a_ptr->velocity.x / mag;
  a_ptr->velocity.y = a_ptr->velocity.y / mag;

}

void
agent_update_mv_avoid(Agent* a_ptr, Agent* t_ptr)
{
  float diff[] = {0.0f, 0.0f};
  float new_vel[] = {0.0f, 0.0f};
  float mag = 0.0f;
  float tmp_fear = a_ptr->dna.fear;

  diff[0] = a_ptr->x - t_ptr->x;
  diff[1] = a_ptr->y - t_ptr->y;

  /* Noramlise and set new vector */
  mag = sqrt((diff[0] * diff[0]) + (diff[1] * diff[1]));
  new_vel[0] = (diff[0] / mag);
  new_vel[1] = (diff[1] / mag);

  /* Take into account metabolism for speed */
  new_vel[0] *= a_ptr->dna.metabolism;
  new_vel[1] *= a_ptr->dna.metabolism;

  if(a_ptr->dna.fear >= 0.0 && t_ptr->state == AGENT_STATE_DEAD)
    tmp_fear = -tmp_fear;

  /* lines */
  if(a_ptr->dna.fear > 0.0){
    glColor4f(1.0, 0.0, 0.0, 0.1);
  }
  else
  {
    glColor4f(0.0, 1.0, 0.0, 0.1);
  }
  glLineWidth((10.0 * tmp_fear * 0.5 - a_ptr->dna.metabolism) + 4.0);
  glBegin(GL_LINES);
  glVertex3f(t_ptr->x, t_ptr->y, 0.0);
  glVertex3f(a_ptr->x, a_ptr->y, 0.0);
  glEnd();
  glLineWidth(1.0);


  if(mag > 0.15){
    a_ptr->velocity.x += new_vel[0] * tmp_fear;
    a_ptr->velocity.y += new_vel[1] * tmp_fear;
  }

  agent_normalize_velocity(a_ptr);
}

void
agent_update_mv_flock(Agent* a_ptr, Agent_array* aa)
{
  int i;
  float total[] = {0.0f, 0.0f};
  float avg[] = {0.0f, 0.0f};
  float new[] = {0.0f, 0.0f};

  if(aa->count == 0) return;
  for(i = 0; i < aa->count; i++) {
    total[0] += aa->agents[i]->velocity.x;
    total[1] += aa->agents[i]->velocity.y;
  }
  printf("total %f, %f\n", total[0], total[1]);
  avg[0] = total[0] / (float) i;
  avg[1] = total[1] / (float) i;

  float mag = sqrt((avg[0] * avg[0]) + (avg[1] * avg[1]));
  new[0] =  avg[0] / mag;
  new[1] =  avg[1] / mag;

  a_ptr->velocity.x += new[0];
  a_ptr->velocity.y += new[1];
  printf("mag %f, %f\n", new[0], new[1]);

  agent_normalize_velocity(a_ptr);


}

void
agents_update_energy(Agent* a_ptr)
{
  a_ptr->energy -= AGENT_METAB_ENERGY_SCALE(a_ptr->dna.metabolism) * AGENTS_TIME_FACTOR;
  if(a_ptr->energy < AGENTS_ENERGY_DEAD) a_ptr->state = AGENT_STATE_DEAD;
}

Agent_verts*
agent_verts_create()
{
  Agent_verts* tmp = malloc(sizeof(Agent_verts));
  tmp->capacity = 16;
  tmp->size = sizeof(float) * 4 * tmp->capacity;;
  tmp->verts_pos = malloc(tmp->size);
  tmp->verts_col = malloc(tmp->size);
  tmp->verts_vis_pos = malloc(tmp->size);
  tmp->verts_vis_col = malloc(tmp->size);
  tmp->a_count = 0;
  tmp->end = 0;
  return tmp;
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

  av->capacity = aa->count;
  av->size = sizeof(float) * 4 * av->capacity;
  av->verts_pos = realloc(av->verts_pos, av->size);
  av->verts_col = realloc(av->verts_col, av->size);
  av->verts_vis_pos = realloc(av->verts_vis_pos, av->size);
  av->verts_vis_col = realloc(av->verts_vis_col, av->size);

  av->end = 0;
  av->a_count= 0;

  // Don't need dynamic array yet
  //  /* if verts array too big, grow */
  //  if(new_size > av->capacity){
  //    av->capacity = new_size;
  //    av->verts_pos = realloc(av->verts_pos, av->capacity);
  //    av->verts_col = realloc(av->verts_col, av->capacity);
  //  }
  //
  for(i = 0; i < aa->count ; i++) {
    Agent* agent = aa->agents[i];
    /* agent drawing */
    float* pos = av->verts_pos;
    float* col = av->verts_col;
    /* vision drawing */
    float* v_pos = av->verts_vis_pos;
    float* v_col = av->verts_vis_col;

    /* rgb, x y and z*/
    v_pos[av->end] = pos[av->end] = agent->x;
    v_col[av->end] = col[av->end] = agent->rgb.r;
    av->end++;

    v_pos[av->end] = pos[av->end] = agent->y;
    v_col[av->end] = col[av->end] = agent->rgb.g;
    av->end++;

    v_pos[av->end] = pos[av->end] = 0.0f;
    v_col[av->end] = col[av->end] = agent->rgb.b;
    av->end++;

    /* special vert data */
    pos[av->end] = AGENT_ENERGY_SIZE_SCALE(agent->energy);
    col[av->end] = (agent->state == AGENT_STATE_PRUNE)?
      0.0f : // pruning
      AGENT_RGB_ALPHA;

    v_pos[av->end] = agent->dna.vision * 400;
    v_col[av->end] = (agent->state != AGENT_STATE_LIVING)?
      0.0f : // pruning
      AGENT_VIS_ALPHA;

    av->end++;
    av->a_count++;
  }
}

Agent_vis_verts*
agent_vis_verts_create()
{
  Agent_vis_verts* tmp = malloc(sizeof(Agent_vis_verts));
  tmp->count = 0;
  tmp->capacity = AGENT_VIS_VERTS_DEFAULT;
  tmp->pos_size = (sizeof(float) * 4 * 4) * tmp->capacity;
  tmp->pos_size = (sizeof(float) * 4 * 4) * tmp->capacity;
  //
}
