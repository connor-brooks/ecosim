#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
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

Agent_array*
agent_array_prune(Agent_array* aa)
{
  int i;
  Agent_array* tmp_aa;
  tmp_aa = agent_array_create();
  for(i = 0; i < aa->count; i++) {
    if(aa->agents[i]->state != AGENT_STATE_PRUNE) {
      agent_array_insert(tmp_aa, aa->agents[i]);
    }
  }
  free(aa);
return tmp_aa;
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
  tmp_agent->dna.rebirth = RANDF_MIN(AGENT_REBIRTH_MIN, AGENT_REBIRTH_MAX);
  tmp_agent->dna.aggresion = RANDF_MIN(AGENT_AGGRESION_MIN, AGENT_AGGRESION_MAX);
  tmp_agent->dna.diet = RANDF_MIN(AGENT_DIET_MIN, AGENT_DIET_MAX);
  tmp_agent->dna.flock = RANDF_MIN(AGENT_FLOCK_MIN, AGENT_FLOCK_MAX);
  tmp_agent->dna.wobble= RANDF_MIN(AGENT_WOBBLE_MIN, AGENT_WOBBLE_MAX);

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
  float *diet = &a_ptr->dna.diet;
  float *flocking = &a_ptr->dna.flock;
  float *metabolism = &a_ptr->dna.metabolism;
  float *rebirth = &a_ptr->dna.rebirth;
  a_ptr->rgb.r = (*diet >= 0)? *diet : 0.0f;
  a_ptr->rgb.g = *flocking; //(a_ptr->dna.diet > 0.0) ? a_ptr->dna.diet : 0.0;
  a_ptr->rgb.b = (*diet < 0.0f)? -(*diet): 0.0f;

  float mag = sqrt(
      (a_ptr->rgb.r * a_ptr->rgb.r) +
      (a_ptr->rgb.g * a_ptr->rgb.g) +
      (a_ptr->rgb.b * a_ptr->rgb.b)
      );

  a_ptr->rgb.r /= mag;
  a_ptr->rgb.g /= mag;
  a_ptr->rgb.b /= mag;
}

void
agents_insert_dead(Agent_array* aa, int count)
{
  int i;
  Agent* tmp_agent;
  for(i = 0; i < count; i++) {
    tmp_agent = agent_create_random();
    tmp_agent->state = AGENT_STATE_DEAD;
    tmp_agent->velocity.x = 0.00f;
    tmp_agent->velocity.y = 0.00f;
    tmp_agent->dna.diet = -1.0f;

    tmp_agent->rgb.r = 0.2;
    tmp_agent->rgb.g = 0.2;
    tmp_agent->rgb.b = 0.2;
    agent_array_insert(aa, tmp_agent);
  }
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
  int i, j;
  Agent* a_ptr;
  Agent_array* local_agents;

  for(i = 0; i < aa->count; i++)
  {
    /* temp pointer for agent */
    a_ptr = aa->agents[i];

//    glColor3f(1.0, 0.0, 0.0);
//    float vis = AGENT_ENERGY_SIZE_SCALE(a_ptr->energy);
//    printf("vis %f\n", vis);
//    glBegin(GL_QUADS);
//    glVertex3f(a_ptr->x, a_ptr->y, 0.0f);
//    glVertex3f(a_ptr->x + vis, a_ptr->y, 0.0f);
//    glVertex3f(a_ptr->x + vis, a_ptr->y + vis, 0.0f);
//    glVertex3f(a_ptr->x , a_ptr->y + vis, 0.0f);
//    glEnd();
//
    // If the agent isn't living, don't update anything
    if(a_ptr->state != AGENT_STATE_LIVING) continue;

    /* get local agents */
    local_agents = agents_get_local(a_ptr, quad, a_ptr->dna.vision);

    /* flock */
    agent_update_mv_flock(a_ptr, local_agents);

    /* avoid or attrack */
    for(j = 0; j < local_agents->count; j++){
      agent_update_mv_avoid(a_ptr, local_agents->agents[j]);
      agent_item_collision(a_ptr, local_agents->agents[j]);
    }

    /* updates */
    agents_update_location(a_ptr);

    agents_update_energy(a_ptr);

    agent_split(a_ptr, aa);

    /* radius sized box around agent */
    agent_setup_vision_quad(a_ptr);
    float *bot_left  = (a_ptr->vis_quad.bot_left);
    float *top_right = (a_ptr->vis_quad.top_right);
    /* debug */
    if(AGENT_DEBUG_SHOW_VISION){
      //quadtree_query_dump(query);
      glColor4f(a_ptr->rgb.r, a_ptr->rgb.g, a_ptr->rgb.b, 0.5);
      glBegin(GL_LINE_LOOP);
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
  float wobble[] = {0.0, 0.0};

  wobble[0] =(2.0 + sin(a_ptr->dna.wobble * glfwGetTime())) * 0.5;
  wobble[1] =(2.0 + sin(a_ptr->dna.wobble * glfwGetTime())) * 0.5;
  a_ptr->x += a_ptr->velocity.x * mv_amt * fabs(wobble[0]);
  a_ptr->y += a_ptr->velocity.y * mv_amt * fabs(wobble[1]);

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
    *tmp_ptr = ((*tmp_ptr > WORLD_MAX_COORD) || (*tmp_ptr <= WORLD_MIN_COORD)) ?
      -(*tmp_ptr)* 0.99:
      *tmp_ptr;
  }
}

float
agent_item_attraction(Agent* a_ptr, Agent* t_ptr, float* mag)
{
  float attraction = 0.0f; //(a_ptr->dna.aggresion);

  int a_diet = (a_ptr->dna.diet > 0.0f) ?
    AGENT_DIET_LIVING :
    AGENT_DIET_DEAD;

  int t_diet = (t_ptr->dna.diet > 0.0f) ?
    AGENT_DIET_LIVING :
    AGENT_DIET_DEAD;

  int t_state = t_ptr->state;

  /* meat eater vs any living */
  if(a_diet == AGENT_DIET_LIVING && t_state == AGENT_STATE_LIVING)
    attraction = 1.0;

  /* meat eater vs other meat eater */
  if(a_diet == AGENT_DIET_LIVING && t_diet == AGENT_DIET_LIVING){
    if(*mag < 0.02 && t_state == AGENT_STATE_LIVING) attraction = -1.0f;
    else attraction = 0.0f;
  }

  /* meat eater vs dead */
  if(a_diet == AGENT_DIET_LIVING && t_state == AGENT_STATE_DEAD)
    attraction = 0.0;

  /* plant eater vs dead */
  if(a_diet == AGENT_DIET_DEAD && t_state == AGENT_STATE_DEAD)
    attraction = 1.0;

  /* plant eater vs platn eater */
  if(a_diet == AGENT_DIET_DEAD && t_diet == AGENT_DIET_DEAD)
  {
    if(*mag < 0.02 && t_state == AGENT_STATE_LIVING) attraction = -1.0f;
    else attraction = 0.0f;
  }

  /* plant eater vs living */
  if(a_diet == AGENT_DIET_DEAD && t_state == AGENT_STATE_LIVING)
    attraction = 0.0;

  /* plant eater vs meat eater*/
  if(a_diet == AGENT_DIET_DEAD && t_diet == AGENT_DIET_LIVING)
    attraction = -1.0;

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
  float attraction;

  /* calc difference */
  diff[0] = a_ptr->x - t_ptr->x;
  diff[1] = a_ptr->y - t_ptr->y;

  /* Noramlise and create new vector */
  mag = sqrt((diff[0] * diff[0]) + (diff[1] * diff[1]));
  new_vel[0] = (diff[0] / mag);
  new_vel[1] = (diff[1] / mag);

  /* Take into account metabolism for speed */
  new_vel[0] *= a_ptr->dna.metabolism / 2.0;
  new_vel[1] *= a_ptr->dna.metabolism / 2.0;

  /* Is agent attracted or repeled? */
  attraction = agent_item_attraction(a_ptr, t_ptr, &mag);

  /* sub from existing vector */
  a_ptr->velocity.x -= new_vel[0] * attraction;
  a_ptr->velocity.y -= new_vel[1] * attraction;

  /* lines */
  glColor4f(a_ptr->rgb.r, a_ptr->rgb.g, a_ptr->rgb.b, 0.25);
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  glVertex3f(t_ptr->x, t_ptr->y, 0.0);
  glVertex3f(a_ptr->x, a_ptr->y, 0.0);
  glEnd();
  glLineWidth(1.0);

  agent_normalize_velocity(a_ptr);
}

void
agent_update_mv_flock(Agent* a_ptr, Agent_array* aa)
{
  int i;
  int count;
  float total[] = {0.0f, 0.0f};
  float avg[] = {0.0f, 0.0f};
  float new[] = {0.0f, 0.0f};
  Agent* tmp_agent;

  float attraction;
  float dummy_f = 1.0f;
  int loop_amt = MIN(8, aa->count);

  if(aa->count == 0) return;

  for(i = 0; i < loop_amt; i++) {
    tmp_agent = aa->agents[i];
    attraction = agent_item_attraction(a_ptr, tmp_agent, &dummy_f);
    //if(aa->agents[i]->state != AGENT_STATE_LIVING) continue;
    if(attraction >= 0){
      total[0] += aa->agents[i]->velocity.x;
      total[1] += aa->agents[i]->velocity.y;
      count++;
    }
  }

  avg[0] = (total[0] == 0) ? 0 : total[0] / (float) count;
  avg[1] = (total[1] == 0) ? 0 : total[1] / (float) count;

  float mag = sqrt((avg[0] * avg[0]) + (avg[1] * avg[1]));
  mag += 0.00001;
  new[0] =  (avg[0] == 0) ? 0 : avg[0] / mag;
  new[1] =  (avg[0] == 0) ? 0 : avg[1] / mag;

  if(mag < 0.20) return;
  a_ptr->velocity.x += new[0] * a_ptr->dna.flock;
  a_ptr->velocity.y += new[1] * a_ptr->dna.flock;

  agent_normalize_velocity(a_ptr);
}

void
agent_item_collision(Agent* a_ptr, Agent* t_ptr)
{
  float close = 0.02;
  //float a_diet = a_ptr->dna.diet;
  //float t_diet = t_ptr->dna.diet;
  int a_diet = (a_ptr->dna.diet > 0.0f) ?
    AGENT_DIET_LIVING :
    AGENT_DIET_DEAD;

  int t_diet = (t_ptr->dna.diet > 0.0f) ?
    AGENT_DIET_LIVING :
    AGENT_DIET_DEAD;



  if((a_ptr->x - close < t_ptr->x) & (a_ptr->x + close > t_ptr->x) &&
      (a_ptr->y - close < t_ptr->y) & (a_ptr->y + close > t_ptr->y)) {

    switch(t_ptr->state){
      case AGENT_STATE_LIVING:
        if(a_diet == AGENT_DIET_LIVING &&
          (t_diet == AGENT_DIET_DEAD))
        {
          t_ptr->state = AGENT_STATE_PRUNE;
          a_ptr->energy += t_ptr->energy;
        }
        break;
      case AGENT_STATE_DEAD:
        if(a_diet == AGENT_DIET_DEAD) {
          t_ptr->state = AGENT_STATE_PRUNE;
          a_ptr->energy += t_ptr->energy;
        }
        break;
    }
  }
}

void
agent_split(Agent* a_ptr, Agent_array* aa)
{
  Agent* tmp_agent;
  /* if energy is higher than rebirth threshold */
  if(a_ptr->energy > a_ptr->dna.rebirth)
  {
    a_ptr->energy *= 0.5;
    tmp_agent = agent_create_random();

    tmp_agent->dna = a_ptr->dna;
    agent_dna_mutate(tmp_agent);

    tmp_agent->x = a_ptr->x + 0.04;
    tmp_agent->y = a_ptr->y + 0.04;

    tmp_agent->velocity.x = a_ptr->velocity.x;
    tmp_agent->velocity.y = a_ptr->velocity.y;

    agent_array_insert(aa, tmp_agent);
  }
}

void
agent_dna_mutate(Agent* a_ptr)
{
  /* SWITCH OVER TO AN ARRAY SO THIS ISNT AS LONG */
  float rate = AGENTS_DNA_MUTATE_RATE;
  float metabolism_change = RANDF_MIN(-1.0, 1.0) * rate;
  float fear_change       = RANDF_MIN(-1.0, 1.0) * rate;
  float vision_change     = RANDF_MIN(-1.0, 1.0) * rate;
  float rebirth_change     = RANDF_MIN(-1.0, 1.0) * rate;
  float aggresion_change     = RANDF_MIN(-1.0, 1.0) * rate;
  float diet_change     = RANDF_MIN(-1.0, 1.0) * rate;
  float flock_change     = RANDF_MIN(-1.0, 1.0) * rate;
  float wobble_change     = RANDF_MIN(-1.0, 1.0) * rate * 10;

  a_ptr->dna.metabolism +=
    (RANDF(1) > 0.3)?
    metabolism_change :
    0;
  a_ptr->dna.fear +=
    (RANDF(1) > 0.3)?
    fear_change:
    0;

  a_ptr->dna.vision +=
    (RANDF(1) > 0.3)?
    vision_change:
    0;

  a_ptr->dna.rebirth +=
    (RANDF(1) > 0.3)?
    rebirth_change :
    0;

  a_ptr->dna.aggresion +=
    (RANDF(1) > 0.3)?
    aggresion_change :
    0;

  a_ptr->dna.diet +=
    (RANDF(1) > 0.3)?
    diet_change :
    0;

  a_ptr->dna.flock +=
    (RANDF(1) > 0.3)?
    flock_change :
    0;

  a_ptr->dna.wobble +=
    (RANDF(1) > 0.3)?
    wobble_change :
    0;

  agent_setup_colors(a_ptr);

  /* use minmax etc with new val for this */
  if(a_ptr->dna.vision < AGENT_VISION_MIN)
    a_ptr->dna.vision = AGENT_VISION_MIN;

  if(a_ptr->dna.vision > AGENT_VISION_MAX)
    a_ptr->dna.vision = AGENT_VISION_MAX;

  if(a_ptr->dna.metabolism < AGENT_METAB_MIN)
    a_ptr->dna.metabolism = AGENT_METAB_MIN;

  if(a_ptr->dna.metabolism > AGENT_METAB_MAX)
    a_ptr->dna.metabolism = AGENT_METAB_MAX;

  if(a_ptr->dna.rebirth < AGENT_REBIRTH_MIN)
    a_ptr->dna.rebirth = AGENT_REBIRTH_MIN;

  if(a_ptr->dna.rebirth > AGENT_REBIRTH_MAX)
    a_ptr->dna.rebirth = AGENT_REBIRTH_MAX;

  if(a_ptr->dna.diet > AGENT_DIET_MAX)
    a_ptr->dna.diet = AGENT_DIET_MAX;

  if(a_ptr->dna.diet < AGENT_DIET_MIN)
    a_ptr->dna.diet = AGENT_DIET_MIN;

  if(a_ptr->dna.flock > AGENT_FLOCK_MAX)
    a_ptr->dna.flock = AGENT_FLOCK_MAX;

  if(a_ptr->dna.flock < AGENT_FLOCK_MIN)
    a_ptr->dna.flock = AGENT_FLOCK_MIN;

  if(a_ptr->dna.wobble > AGENT_WOBBLE_MAX)
    a_ptr->dna.wobble = AGENT_WOBBLE_MAX;

  if(a_ptr->dna.wobble < AGENT_WOBBLE_MIN)
    a_ptr->dna.wobble = AGENT_WOBBLE_MIN;

}

void
agents_update_energy(Agent* a_ptr)
{
  a_ptr->energy -= AGENT_METAB_ENERGY_SCALE(a_ptr->dna.metabolism) * AGENTS_TIME_FACTOR;
  if(a_ptr->energy < AGENTS_ENERGY_DEAD) {
    a_ptr->state = AGENT_STATE_DEAD;
    a_ptr->velocity.x = 0.0f;
    a_ptr->velocity.y = 0.0f;
    a_ptr->dna.diet = -1.0f;
  }
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
