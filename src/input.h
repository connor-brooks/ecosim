#ifndef INPUT_H
#define INPUT_H
#include "agents.h"
#include "config.h"

typedef struct _Input Input;

struct _Input {
  struct _btn {
    int is_down;
  } btn_left, btn_right;

  struct _spawner {
  void* last_ptr;
  float last_time;
  float pos[2];
  } spawner;
};

Input* input_create();
void input_free(Input* input);
void input_spawn_begin(Input* input, float pos[]);
void input_spawn_end(Input* input);
void input_spawn_cycle(Input* input, Agent_array* aa);
#endif
