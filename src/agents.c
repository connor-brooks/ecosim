#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include "config.h"
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
  temp->clock = 0;

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

/* Remove non-visable agents from the array */
Agent_array*
agent_array_prune(Agent_array* aa)
{
  int i;
  Agent_array* tmp_aa;
  tmp_aa = agent_array_create();
  tmp_aa->clock = aa->clock;
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
  tmp_agent->dna.vision = RANDF_MIN(AGENT_VISION_MIN, AGENT_VISION_MAX);
  tmp_agent->dna.rebirth = RANDF_MIN(AGENT_REBIRTH_MIN, AGENT_REBIRTH_MAX);
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


  float *rebirth = &a_ptr->dna.rebirth;
  a_ptr->rgb.r = (agent_diet(a_ptr) == AGENT_DIET_LIVING)?
    *diet:
    0.0f;
  a_ptr->rgb.g = *flocking;
  a_ptr->rgb.b = (agent_diet(a_ptr))?
    1.0 - (*diet) :
    0.0f;

  /* Normalise color vector */
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
agent_array_to_quadtree(Agent_array* aa, Quadtree* q)
{
  int i;
  Agent* tmp_agent;
  for(i = 0; i < aa->count; i++) {
    tmp_agent = (aa->agents[i]);
    float tmp_pos[] = {tmp_agent->x, tmp_agent->y};
    if(tmp_agent->state != AGENT_STATE_PRUNE)
      quadtree_insert(q, tmp_agent, tmp_pos);
  }
}


int
agents_food_drop(Agent_array* aa, float time, float last)
{
  int ret = 0;
  int food_insert_amount = (int) RANDF_MIN(DEV_GAME_FOOD_SPAWN_MIN,
      DEV_GAME_FOOD_SPAWN_MAX);

  if(time > last + DEV_GAME_FOOD_SPAWN_FREQ) {
    agents_insert_dead(aa, food_insert_amount);
    printf("Food added \n");
    ret = 1;
  }
  return ret;
}


/* Insert "dead" agents into the array so food */
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
    tmp_agent->energy *= DEV_GAME_FOOD_ENERGY;

    tmp_agent->rgb.r = 0.2;
    tmp_agent->rgb.g = 0.2;
    tmp_agent->rgb.b = 0.2;
    agent_array_insert(aa, tmp_agent);
  }
}

/* Calculate vision quad area */
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

/* Create a empty array and fill it with agents */
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

/* Destroy agent array */
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

  //  printf("time %f clock %f\n", glfwGetTime(), aa->clock);
  aa->clock += 0.05;

  for(i = 0; i < aa->count; i++)
  {
    /* temp pointer for agent */
    a_ptr = aa->agents[i];

    // If the agent isn't living, don't update anything
    if(a_ptr->state != AGENT_STATE_LIVING) continue;

    /* get local agents */
    local_agents = agents_get_local(a_ptr, quad, a_ptr->dna.vision);

    /* flock */
    agent_mv_flock(a_ptr, local_agents);
    agent_normalize_velocity(a_ptr);

    /* avoid or attrack */
    for(j = 0; j < local_agents->count; j++){
      agent_update_mv_avoid(a_ptr, local_agents->agents[j]);
      agent_item_collision(a_ptr, local_agents->agents[j]);
    }

    /* updates */
    agents_update_location(a_ptr, aa->clock);
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

/* Query the quadtree for local agents,
 * Check they are in line of sight
 * Ignore irrelevent agents */
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

/* Move the agent based on their velocity */
void
agents_update_location(Agent* a_ptr, float clock)
{
  float mv_amt = agents_update_mv_amt(a_ptr);
  float wobble[] = {0.0, 0.0};

  /* add a wobble to their step */
  wobble[0] =(2.0 + sin(a_ptr->dna.wobble + a_ptr->dna.wobble * clock)) * 0.5;
  wobble[1] =(2.0 + sin(a_ptr->dna.wobble + a_ptr->dna.wobble * clock)) * 0.5;
  a_ptr->x += a_ptr->velocity.x * mv_amt * fabs(wobble[0]);
  a_ptr->y += a_ptr->velocity.y * mv_amt  * fabs(wobble[1]);

  /* Wrap if needed */
  agents_update_mv_wrap(a_ptr);
}

/* Calculate how much the agent can move */
float
agents_update_mv_amt(Agent* a_ptr)
{
  return AGENT_MAX_SPEED * a_ptr->dna.metabolism;
}

/* Wraps agents to the other side of the screen if they leave the visible
 * area.
 * This function is stupid.
 * Put X and y for agents into an arrary so this type
 * of thing doesn't exist. */
void
agents_update_mv_wrap(Agent* a_ptr)
{
  float* tmp_ptr;
  /* loop through x and y and put the agents at the opposite side
   * of the screen if they're past the ends */
  for(tmp_ptr = &a_ptr->x; tmp_ptr <= &a_ptr->y; tmp_ptr++){
    *tmp_ptr = ((*tmp_ptr > WORLD_MAX_COORD) ||
        (*tmp_ptr <= WORLD_MIN_COORD)) ?
      -(*tmp_ptr)* 0.99:
      *tmp_ptr;
  }
}

/* Calculate the attitude of one agent about another*/
float
agent_item_attraction(Agent* a_ptr, Agent* t_ptr)
{
  float attraction = AGENT_NEUTRAL;

  int a_diet = agent_diet(a_ptr);

  int t_diet = agent_diet(t_ptr);

  int t_state = t_ptr->state;

  if(t_state == AGENT_STATE_PRUNE) return AGENT_NEUTRAL;

  /* meat eater vs other meat eater */
  else if(a_diet == AGENT_DIET_LIVING && t_diet == AGENT_DIET_LIVING)
    attraction = AGENT_NEUTRAL;

  /* meat eater vs any living */
  else if(a_diet == AGENT_DIET_LIVING && t_state == AGENT_STATE_LIVING)
    attraction = AGENT_ATTRACT;

  /* meat eater vs dead */
  else if(a_diet == AGENT_DIET_LIVING && t_state == AGENT_STATE_DEAD)
    attraction = AGENT_NEUTRAL;

  /* plant eater vs dead */
  else if(a_diet == AGENT_DIET_DEAD && t_state == AGENT_STATE_DEAD)
    attraction = AGENT_ATTRACT;

  /* plant eater vs platn eater */
  else if(a_diet == AGENT_DIET_DEAD && t_diet == AGENT_DIET_DEAD)
    attraction = AGENT_NEUTRAL;

  /* plant eater vs meat eater*/
  else if(a_diet == AGENT_DIET_DEAD && t_diet == AGENT_DIET_LIVING)
    attraction = AGENT_AVOID;

  /* plant eater vs living */
  else if(a_diet == AGENT_DIET_DEAD && t_state == AGENT_STATE_LIVING)
    attraction = AGENT_NEUTRAL;

  return attraction;
}

/* Normalise velocity */
void
agent_normalize_velocity(Agent* a_ptr)
{
  float mag = sqrt((a_ptr->velocity.x * a_ptr->velocity.x)
      + (a_ptr->velocity.y * a_ptr->velocity.y));
  a_ptr->velocity.x = a_ptr->velocity.x / mag;
  a_ptr->velocity.y = a_ptr->velocity.y / mag;
}

/* Move agent with respect to other entities */
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

  /* Is agent attracted or repeled? */
  attraction = agent_item_attraction(a_ptr, t_ptr);

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

/* Flock agents */
void agent_mv_flock(Agent* a_ptr, Agent_array* aa)
{
  float final_vel[] = {0.0f, 0.0f};
  float* align_vel = agent_mv_flock_align(a_ptr, aa);
  float* cohes_vel = agent_mv_flock_cohesion(a_ptr, aa);
  float* serpa_vel = agent_mv_flock_seperation(a_ptr, aa);

  /* Velocity alignment */
  final_vel[0] += align_vel[0];
  final_vel[1] += align_vel[1];

  /* Cohesion: Go to center of mass */
  final_vel[0] += cohes_vel[0];
  final_vel[1] += cohes_vel[1];

  /* Seperation: Avoid otherss */
  final_vel[0] += serpa_vel[0];
  final_vel[1] += serpa_vel[1];

  free(align_vel);
  free(cohes_vel);
  free(serpa_vel);

  a_ptr->velocity.x += final_vel[0] * a_ptr->dna.flock;
  a_ptr->velocity.y += final_vel[1] * a_ptr->dna.flock;
  //  agent_normalize_velocity(a_ptr);

}

/* Align with neutral agents */
float*
agent_mv_flock_align(Agent* a_ptr, Agent_array* aa)
{
  int i;
  int count = 0;
  float* return_vel = malloc(sizeof(float) * 2);
  float total[] = {0.0f, 0.0f};
  float avg[] = {0.0f, 0.0f};
  float mag;
  float attraction;

  return_vel[0] = 0.0f;
  return_vel[1] = 0.0f;

  /* Get total */
  for(i = 0; i < aa->count; i++) {
    attraction = agent_item_attraction(a_ptr, aa->agents[i]);
    if(aa->agents[i]->state == AGENT_STATE_LIVING && attraction == 0)
    {
      total[0] += aa->agents[i]->velocity.x;
      total[1] += aa->agents[i]->velocity.y;
      count++;
    }
  }
  if(count == 0) return return_vel;

  /* Get average */
  avg[0] = total[0] / (float) count;
  avg[1] = total[1] / (float) count;

  /* Normalise */
  mag = sqrt((avg[0] * avg[0]) + (avg[1] * avg[1]));
  if(mag == 0) return return_vel;
  return_vel[0] = avg[0] / mag;
  return_vel[1] = avg[1] / mag;

  return return_vel;
}

/* Go towards center of mass */
float*
agent_mv_flock_cohesion(Agent* a_ptr, Agent_array* aa)
{
  int i;
  int count = 0;
  float* return_vel = malloc(sizeof(float) * 2);
  float total[] = {0.0f, 0.0f};
  float avg[] = {0.0f, 0.0f};
  float mass_dir[] = {0.0f, 0.0f};
  float mag;
  float attraction;

  return_vel[0] = 0.0f;
  return_vel[1] = 0.0f;

  /* Get total */
  for(i = 0; i < aa->count; i++) {
    attraction = agent_item_attraction(a_ptr, aa->agents[i]);
    if(aa->agents[i]->state == AGENT_STATE_LIVING &&
        attraction == AGENT_NEUTRAL)
    {
      total[0] += aa->agents[i]->x;
      total[1] += aa->agents[i]->y;
      count++;
    }
  }
  if(count == 0) return return_vel;

  /* Get average */
  avg[0] = total[0] / (float) count;
  avg[1] = total[1] / (float) count;

  /* Direction to center of mass */
  mass_dir[0] = avg[0] - a_ptr->x;
  mass_dir[1] = avg[1] - a_ptr->y;

  /* Normalise */
  mag = sqrt((mass_dir[0] * mass_dir[0]) + (mass_dir[1] * mass_dir[1]));
  if(mag == 0) return return_vel;
  return_vel[0] = mass_dir[0] / mag;
  return_vel[1] = mass_dir[1] / mag;

  return return_vel;

}

/* Seperate from each other */
float*
agent_mv_flock_seperation(Agent* a_ptr, Agent_array* aa)
{
  int i;
  int count = 0;
  float* return_vel = malloc(sizeof(float) * 2);
  float total[] = {0.0f, 0.0f};
  float avg[] = {0.0f, 0.0f};
  float mag;
  float attraction;

  return_vel[0] = 0.0f;
  return_vel[1] = 0.0f;

  /* Get total */
  for(i = 0; i < aa->count; i++) {
    attraction = agent_item_attraction(a_ptr, aa->agents[i]);
    if(aa->agents[i]->state == AGENT_STATE_LIVING &&
        attraction == AGENT_NEUTRAL)
    {
      total[0] += aa->agents[i]->x - a_ptr->x;
      total[1] += aa->agents[i]->y - a_ptr->y;
      count++;
    }
  }
  if(count == 0) return return_vel;

  /* Get average */
  avg[0] = total[0] / (float) count;
  avg[1] = total[1] / (float) count;

  avg[0] *= -1;
  avg[1] *= -1;

  /* Normalise */
  mag = sqrt((avg[0] * avg[0]) + (avg[1] * avg[1]));
  if(mag == 0) return return_vel;
  return_vel[0] = avg[0] / mag;
  return_vel[1] = avg[1] / mag;

  return return_vel;
}

/* Act on collision of two agents */
void
agent_item_collision(Agent* a_ptr, Agent* t_ptr)
{
  float close = 0.02;

  int a_diet = agent_diet(a_ptr);
  int t_diet = agent_diet(t_ptr);


  /* Is the agent close enough to the target? */
  if(!((a_ptr->x - close < t_ptr->x) & (a_ptr->x + close > t_ptr->x) &&
        (a_ptr->y - close < t_ptr->y) & (a_ptr->y + close > t_ptr->y))) {
    return;
  }

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

/* Create a copy of self */
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

    tmp_agent->x = a_ptr->x + 0.06;
    tmp_agent->y = a_ptr->y + 0.06;

    tmp_agent->velocity.x = a_ptr->velocity.x;
    tmp_agent->velocity.y = a_ptr->velocity.y;

    agent_array_insert(aa, tmp_agent);
  }
}

void
agent_dna_mutate_trait(float* trait, float rate, float probability)
{
  *trait +=
    (RANDF(1) > probability)?
    RANDF_MIN(-1.0, 1.0) * rate :
    0;
}

void
agent_dna_trait_constrain(float* trait, float min, float max)
{
  if(*trait < min) *trait = min;
  else if(*trait > max) *trait = max;
}

void
agent_dna_mutate(Agent* a_ptr)
{
  float rate = AGENT_DNA_MUTATE_RATE;

  /* Mutate traits */
  agent_dna_mutate_trait(&(a_ptr->dna.metabolism), rate, 0.3);
  agent_dna_mutate_trait(&(a_ptr->dna.vision), rate, 0.3);
  agent_dna_mutate_trait(&(a_ptr->dna.rebirth), rate, 0.3);
  agent_dna_mutate_trait(&(a_ptr->dna.diet), rate, 0.3);
  agent_dna_mutate_trait(&(a_ptr->dna.flock), rate, 0.3);
  agent_dna_mutate_trait(&(a_ptr->dna.wobble), rate, 0.3);

  /* Stop going over max or under min */
  agent_dna_trait_constrain(&(a_ptr->dna.vision),
      AGENT_VISION_MIN, AGENT_VISION_MAX);
  agent_dna_trait_constrain(&(a_ptr->dna.metabolism),
      AGENT_METAB_MIN, AGENT_METAB_MAX);
  agent_dna_trait_constrain(&(a_ptr->dna.rebirth),
      AGENT_REBIRTH_MIN, AGENT_REBIRTH_MAX);
  agent_dna_trait_constrain(&(a_ptr->dna.diet),
      AGENT_DIET_MIN, AGENT_DIET_MAX);
  agent_dna_trait_constrain(&(a_ptr->dna.flock),
      AGENT_FLOCK_MIN, AGENT_FLOCK_MAX);
  agent_dna_trait_constrain(&(a_ptr->dna.wobble),
      AGENT_WOBBLE_MIN, AGENT_WOBBLE_MAX);

  agent_setup_colors(a_ptr);
}

void
agents_update_energy(Agent* a_ptr)
{
  a_ptr->energy -= AGENT_METAB_ENERGY_SCALE(a_ptr->dna.metabolism) *
    AGENT_TIME_FACTOR;
  if(a_ptr->energy < AGENT_ENERGY_DEAD) {
    a_ptr->state = AGENT_STATE_DEAD;
    a_ptr->velocity.x = 0.0f;
    a_ptr->velocity.y = 0.0f;
    a_ptr->dna.diet = -1.0f;
    a_ptr->rgb.r = 0.2;
    a_ptr->rgb.g = 0.2;
    a_ptr->rgb.b = 0.2;
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

  av->end = 0;
  av->a_count= 0;

  for(i = 0; i < aa->count ; i++) {
    Agent* agent = aa->agents[i];
    /* agent drawing */
    float* pos = av->verts_pos;
    float* col = av->verts_col;

    /* rgb, x y and z*/
    pos[av->end] = agent->x;
    col[av->end] = agent->rgb.r;
    av->end++;

    pos[av->end] = agent->y;
    col[av->end] = agent->rgb.g;
    av->end++;

    pos[av->end] = AGENT_ENERGY_SIZE_SCALE(agent->energy);
    col[av->end] = agent->rgb.b;
    av->end++;

    /* special vert data */
    pos[av->end] = agent->dna.vision * 400;
    col[av->end] = agent->state;

    av->end++;
    av->a_count++;
  }
}

int
agent_diet(Agent* tmp_agent)
{
  int diet = (tmp_agent->dna.diet >= AGENT_DIET_BOUNDARY) ?
    AGENT_DIET_LIVING :
    AGENT_DIET_DEAD;
  return diet;

}
