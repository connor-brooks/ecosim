#ifndef AGENTS_H
#define AGENTS_H
#include <stdio.h>
#include <stddef.h>
#include "utils.h"
#include "quadtree.h"

typedef struct _Agent Agent;
typedef struct _Agent_array Agent_array;
typedef struct _Agent_verts Agent_verts;
typedef struct _DNA DNA;

enum agent_states {
  AGENT_STATE_PRUNE  = -1,
  AGENT_STATE_DEAD   =  0,
  AGENT_STATE_LIVING =  1
};

enum agent_diet {
  AGENT_DIET_LIVING,
  AGENT_DIET_DEAD
};

enum agent_attraction {
  AGENT_AVOID   = -1,
  AGENT_NEUTRAL =  0,
  AGENT_ATTRACT =  1
};

struct _DNA {
  float metabolism;
  float vision;
  float rebirth;
  float diet; /* -1 for dead, 1 for living */
  float flock;
  float wobble;

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
  DNA dna;

  struct {
    float bot_left[2];
    float top_right[2];
  } vis_quad;


};

struct _Agent_array {
  Agent** agents;
  size_t count;
  size_t capacity;
  size_t size;
  int count_change;
  float clock;
};


struct _Agent_verts {
  float* verts_pos, *verts_col;
  size_t capacity;
  size_t size;
  int a_count;
  ptrdiff_t end;
};


Agent_array* agent_array_create();
Agent_array* agent_array_setup_random(int count);
void agent_array_insert(Agent_array* aa, Agent* a);
void agent_array_free(Agent_array* aa);
Agent_array* agent_array_prune(Agent_array* aa);
void agents_update(Agent_array* aa, Quadtree* quad);

void agent_array_to_quadtree(Agent_array* aa, Quadtree* q);


Agent_array* agents_get_local(Agent* a_ptr, Quadtree* quad, float radius);

Agent* agent_create_random();
void agent_setup_colors(Agent* a_ptr);
void agent_setup_vision_quad(Agent* a_ptr);
void agents_update_location(Agent* a_ptr, float clock);
void agents_update_energy(Agent* a_ptr);
float agents_update_mv_amt(Agent* a_ptr);
void agents_update_mv_wrap(Agent* a_ptr);
void agents_insert_dead(Agent_array* aa, int count);

int agents_food_drop(Agent_array* aa, float time, float last);

void agent_normalize_velocity(Agent* a_ptr);
void agent_update_mv_avoid(Agent* a_ptr, Agent* t_ptr);
float agent_item_attraction(Agent* a_ptr, Agent* t_ptr);

void agent_mv_flock(Agent* a_ptr, Agent_array* aa);
float* agent_mv_flock_align(Agent* a_ptr, Agent_array* aa);
float* agent_mv_flock_cohesion(Agent* a_ptr, Agent_array* aa);
float* agent_mv_flock_seperation(Agent* a_ptr, Agent_array* aa);

void agent_item_collision(Agent* a_ptr, Agent* t_ptr);

void agent_split(Agent* a_ptr, Agent_array* aa);
void agent_dna_mutate(Agent* a_ptr);
void agent_dna_mutate_trait(float* trait, float rate, float probability);
void agent_dna_trait_constrain(float* trait, float min, float max);

/* verts */
Agent_verts* agent_verts_create();
void agent_verts_free(Agent_verts* av);
void agents_to_verts(Agent_array* aa, Agent_verts* av);

int agent_diet(Agent* tmp_agent);

#endif

