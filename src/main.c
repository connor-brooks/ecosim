#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "config.h"
#include "agents.h"
#include "utils.h"
#include "input.h"
#include "graphics.h"
#include "quadtree.h"
#include "log.h"


int game_run = 1;

/* For passing pointers to callbacks */
struct Callback_ptrs{
  Agent_array* aa;
  Input* inp;
  World_view* wv;
  Framebuffer* fb;
  Framebuffer** fb_ptr;
};

/* Keyboard callback */
void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    /* Pause the simulation */
    if(key == GLFW_KEY_SPACE)
      game_run = !game_run;

    /* Close the simulation */
    else if(key == GLFW_KEY_Q)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  struct Callback_ptrs* callb_ptrs;
  Input* input;
  World_view* wv ;
  double xPos, yPos;

  /* Grab pointers of structs*/
  callb_ptrs = glfwGetWindowUserPointer(window);
  input = callb_ptrs->inp;
  wv = callb_ptrs->wv;

  /* Begin a spawn cycle if mouse is pressed */
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    glfwGetCursorPos(window, &xPos, &yPos);
    float* relPos = gfx_world_view_relpos(wv, window, xPos, yPos);
    input_spawn_begin(input, relPos);
    free(relPos);
  }
  /* End spawn cycle when mouse is release */
  else if(button == GLFW_MOUSE_BUTTON_LEFT  && action == GLFW_RELEASE)
    input_spawn_end(input);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  struct Callback_ptrs* callb_ptrs;
  int key;
  World_view* wv;

  /* grab pointers */
  callb_ptrs = glfwGetWindowUserPointer(window);
  wv = callb_ptrs->wv;

  /* is ctrl clicked */
  key = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);

  /* Zoom if ctl is pressed, scroll otherwise */
  if(key){
    gfx_world_view_zoom(wv, xoffset, yoffset);

  } else {
    gfx_world_view_scroll(wv, xoffset, yoffset);
  }
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
  struct Callback_ptrs* callb_ptrs;
  Framebuffer** fb_ptr;

  /* Grab pointers*/
  callb_ptrs = glfwGetWindowUserPointer(window);
  fb_ptr = callb_ptrs->fb_ptr;
  /* Rebuild framebuffer to width x height */
  *fb_ptr = gfx_framebuffer_create(width, height);
}

int
main(int argc, char **argv)
{
  GLFWwindow* window;
  Framebuffer* framebuffer;
  World_view* world_view;
  Input* input;
  Agent_array* agent_array;
  Agent_verts* agent_verts;
  Quadtree* quad;
  Logger* logger;
  int cyclecount = 0;
  float quad_head_pos[] = {-1.0f, -1.0f};
  float quad_head_size = 2.0f;
  float last_update_time = glfwGetTime();
  float last_food_time = last_update_time;
  // For passing structs between callbacks in glfw
  struct Callback_ptrs callb_ptrs;

  /* Initalize glfw and glut*/
  if (!glfwInit())
    return -1; //exit

  /* Create window */
  window = glfwCreateWindow(1600, 900, "ecosim", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Set the windows context */
  glfwMakeContextCurrent(window);

  /* initalize glew and do various gl setup */
  srand((unsigned int) time(NULL));
  glewInit();
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Setup world view */
  framebuffer = gfx_framebuffer_create(1600, 900);
  GLuint fb_shader = gfx_framebuffer_shader();
  world_view = gfx_world_view_create();
  input = input_create();

  logger= logger_create();

  /* Setup shaders, agents and verts */
  agent_array = agent_array_setup_random(DEV_AGENT_COUNT);
  agents_insert_dead(agent_array, DEV_GAME_FOOD_SPAWN_INIT);
  agent_verts = agent_verts_create();
  GLuint agent_shader = gfx_agent_shader();
  GLuint agent_vis_shader = gfx_agent_vis_shader();
  GLuint bg_shader = gfx_bg_shader();

  /* Setup callbacks */
  callb_ptrs.inp = input;
  callb_ptrs.aa = agent_array;
  callb_ptrs.wv = world_view;
  callb_ptrs.fb = framebuffer;
  callb_ptrs.fb_ptr = &framebuffer;
  glfwSetWindowUserPointer(window, &callb_ptrs);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowSizeCallback(window, window_size_callback);

  /* Play background noise */
  system("ffplay ambient.wav -loop 0 -nodisp -loglevel quiet &");

  /* Main loop */
  while(!glfwWindowShouldClose(window))
  {
    /* Set the viewport & grab scale*/
    int i;
    Agent* tmp_agent;
    int width, height;
    float scale;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    scale = gfx_get_scale(window);

    /* Deal with user input */
    if(input->btn_left.is_down) {
      input_spawn_cycle(input, agent_array);
      agents_to_verts(agent_array, agent_verts);
    }

    /* Main update cycle */
    if(game_run && glfwGetTime() > last_update_time + (1.0 / DEV_GAME_FPS))
    {
      last_update_time = glfwGetTime();

      /* Recreate quadtree and insert agents */
      quad = quadtree_create(quad_head_pos, quad_head_size);
      for(i = 0; i < agent_array->count; i++) {
        tmp_agent = (agent_array->agents[i]);
        float tmp_pos[] = {tmp_agent->x, tmp_agent->y};
        if(tmp_agent->state != AGENT_STATE_PRUNE)
          quadtree_insert(quad, tmp_agent, tmp_pos);
      }
      /* Update agents position using quadtree, convert
       * them into their verts*/
      agents_update(agent_array, quad);
      agents_to_verts(agent_array, agent_verts);

      /* Insert dead agents for herbivours */
      if(glfwGetTime() > last_food_time + DEV_GAME_FOOD_SPAWN_FREQ) {
        int food_insert_amount = (int) RANDF_MIN(DEV_GAME_FOOD_SPAWN_MIN,
            DEV_GAME_FOOD_SPAWN_MAX);
        agents_insert_dead(agent_array, food_insert_amount);
        printf("Food added & agent array pruned\n");

        agent_array = agent_array_prune(agent_array);
        int veg = 0;
        int meat = 0;
        for(int test = 0; test < agent_array->count; test++) {
          if(agent_array->agents[test]->state == AGENT_STATE_LIVING){
            if(agent_array->agents[test]->dna.diet > 0) meat++;
            else veg++;
          }
        }

        printf("time_");
        logger_record(logger, LOG_TIME, (int) glfwGetTime());
        printf("pop_");
        logger_record(logger, LOG_POPULATION, (int)  agent_array->count);
        printf("veg_");
        logger_record(logger, LOG_HERBIVOUR, (int) veg);
        printf("meat_");
        logger_record(logger, LOG_CARNIVOUR, (int)  meat);

        callb_ptrs.aa = agent_array;
        last_food_time = glfwGetTime();
      }
      cyclecount++;

      quadtree_free(quad);
    }

    /* Draw all the elments to off-screen framebuffer */
    glClear(GL_COLOR_BUFFER_BIT);
    gfx_framebuffer_begin(framebuffer, world_view);
    gfx_bg_draw(bg_shader, glfwGetTime());
    gfx_agents_draw_cell(agent_verts, agent_shader,
        scale, world_view->zoom);
    gfx_agents_draw_vis(agent_verts, agent_vis_shader,
        scale, world_view->zoom);
    gfx_framebuffer_end();

    /* Draw a shaded version of the framebuffer */
    gfx_framebuffer_draw(framebuffer, world_view, fb_shader);

    /* swap */
    glfwSwapBuffers(window);
    /* Poll for events */
    glfwPollEvents();
  }

  /* Agent verts can be persistant, so free at end, not each frame*/
  logger_write(logger);
  logger_free(logger);
  agent_array_free(agent_array);
  agent_verts_free(agent_verts);
  free(framebuffer);
  input_free(input);
  system("pkill ffplay");
  printf("KILLED\n");
  glfwTerminate();
  return 0;
}
