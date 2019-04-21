#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "input.h"
#include "config.h"
#include "agents.h"

Input*
input_create()
{
  Input* tmp;
  tmp = malloc(sizeof(Input));
  tmp->btn_left.is_down = 0;
  tmp->btn_right.is_down = 0;

  tmp->spawner.last_ptr = NULL;
  tmp->spawner.last_time = 0;
  tmp->spawner.pos[0] = 0;
  tmp->spawner.pos[1] = 0;

  return tmp;
}

void
input_spawn_begin(Input* input, float pos[])
{
  input->btn_left.is_down = 1;
  input->spawner.last_time = glfwGetTime() - INPUT_SPAWN_DELAY;
  input->spawner.pos[0] = pos[0];
  input->spawner.pos[1] = pos[1];
}

void
input_spawn_end(Input* input)
{
  input->btn_left.is_down = 0;
  input->spawner.last_time = 0;
  input->spawner.last_ptr = NULL;
}

void
input_spawn_cycle(Input* input, Agent_array* aa)
{
  float time_cur = glfwGetTime();
  float* time_last = &(input->spawner.last_time);
  float* spawn_pos = input->spawner.pos;
  Agent* agent_last = input->spawner.last_ptr;
  Agent* agent_tmp;

  /* Spawn a new agent once the mouse has been held
   * for X amount of seconds */
  if(time_cur - *time_last  >= INPUT_SPAWN_DELAY)
  {
    /* Remove the old agent if one exits */
    if(agent_last != NULL)
    {
      agent_last->state = AGENT_STATE_PRUNE;
    }

    /* Create and insert the new agent */
    agent_tmp = agent_create_random();
    agent_tmp->x = spawn_pos[0];
    agent_tmp->y = spawn_pos[1];
    agent_array_insert(aa, agent_tmp);
    input->spawner.last_ptr = agent_tmp;

    /* Set the lsat spanwed time */
    input->spawner.last_time = time_cur;
  }
}

void
input_free(Input* input)
{
  free(input);
}
