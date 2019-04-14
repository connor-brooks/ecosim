#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "agents.h"
#include "utils.h"
#include "graphics.h"
#include "quadtree.h"

#define DEV_AGENT_COUNT (80)


/* TEMPORARY GLOBAL */
int game_run = 1;
int food_spawn_freq_mod = 0;

/* For passing pointers to callbacks */
struct Callback_ptrs{
  Agent_array* aa;
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
    if(key == GLFW_KEY_SPACE)
      game_run = !game_run;
    if(key == GLFW_KEY_Q)
      glfwSetWindowShouldClose(window, GLFW_TRUE);

    if(key == GLFW_KEY_DOWN)
      food_spawn_freq_mod -=1;

    if(key == GLFW_KEY_UP)
      food_spawn_freq_mod +=1;

    printf("spawn %d\n", food_spawn_freq_mod);
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    struct Callback_ptrs* callb_ptrs;
    Agent_array* aa;
    World_view* wv ;
    double xPos, yPos;
    Agent* tmp_agent;

    callb_ptrs = glfwGetWindowUserPointer(window);
    aa = callb_ptrs->aa;
    wv = callb_ptrs->wv;

    glfwGetCursorPos(window, &xPos, &yPos);

    float* relPos = gfx_world_view_relpos(wv, window, xPos, yPos);

    tmp_agent = agent_create_random();
    tmp_agent->x = relPos[0];
    tmp_agent->y = relPos[1];
    free(relPos);
    agent_array_insert(aa, tmp_agent);
  }

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

  /* Rebuild framebuffer to width x height */
  callb_ptrs = glfwGetWindowUserPointer(window);
  fb_ptr = callb_ptrs->fb_ptr;
  *fb_ptr = gfx_framebuffer_create(width, height);


}

int
main(int argc, char **argv)
{
  GLFWwindow* window;
  Framebuffer* framebuffer;
  World_view* world_view;
  Agent_array* agent_array;
  Agent_verts* agent_verts_new;
  Quadtree* quad;
  Quadtree_verts* quad_verts;
  int cyclecount = 0;
  float quad_head_pos[] = {-1.0f, -1.0f};
  float quad_head_size = 2.0f;
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
  glewInit();
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  srand((unsigned int)time(NULL));

  /* Setup world view */
  framebuffer = gfx_framebuffer_create(1600, 900);
  GLuint fb_shader = gfx_framebuffer_shader();
  world_view = gfx_world_view_create();

  /* Setup shaders, agents and verts */
  agent_array = agent_array_setup_random(DEV_AGENT_COUNT);
  agents_insert_dead(agent_array, 25);
  agent_verts_new = agent_verts_create();
  GLuint agent_shader = gfx_agent_shader();
  GLuint agent_vis_shader = gfx_agent_vis_shader();
  GLuint world_shader = gfx_world_shader();

  /* Setup callbacks */
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
    float tmp_pos;
    int width, height;
    float scale;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    scale = gfx_get_scale(window);

    /* Clear*/
    glClear(GL_COLOR_BUFFER_BIT);

    /* Recreate quadtree and insert agents */
    quad = quadtree_create(quad_head_pos, quad_head_size);

    /* Insert agents into quadtree */
    for(i = 0; i < agent_array->count; i++) {
      tmp_agent = (agent_array->agents[i]);
      float tmp_pos[] = {tmp_agent->x, tmp_agent->y};
      if(tmp_agent->state != AGENT_STATE_PRUNE)
        quadtree_insert(quad, tmp_agent, tmp_pos);
    }

    /* QUADTREE GRAPHICS ARE BEING REMOVED SOON
     * Every frame rebuild the quadtree verts, draw them free them
     * will sort out later */
    //quad_verts = quadtree_verts_create();
    //quadtree_to_verts(quad, quad_verts);
    //gfx_quad_draw(quad_verts);
    //quadtree_verts_free(quad_verts);

    /* Convert agents to verts and draw them
     * This function should only rebuild the verts array IF the agent count has changed,
     * so we free at end, as the struct should presist all through running of program*/
    agents_to_verts(agent_array, agent_verts_new);

    /* Draw all the elments to framebuffer */
    gfx_framebuffer_begin(framebuffer, world_view);
    gfx_world_texture(world_shader, glfwGetTime());
    gfx_agents_draw_new(agent_verts_new, agent_shader, scale, world_view->zoom);
    gfx_agents_draw_vis(agent_verts_new, agent_vis_shader, scale, world_view->zoom);
    gfx_framebuffer_end();

    /* Draw shaded framebuffer */
    gfx_framebuffer_draw(framebuffer, world_view, fb_shader);

    /* Update */
    if(game_run)
    {
      agents_update(agent_array, quad);
      /* insert food agents every 100 cycles */
      // Convert to time, don't use cycles 
      if(cyclecount % (100 + food_spawn_freq_mod) == 0) {
        agents_insert_dead(agent_array, RANDF_MIN(5, 7));
        printf("Food added & agent array pruned\n");
        agent_array = agent_array_prune(agent_array);
        callb_ptrs.aa = agent_array;
      }
      cyclecount++;
    }

    quadtree_free(quad);
    /* swap */
    glfwSwapBuffers(window);
    /* Poll for events */
    glfwPollEvents();
  }

  /* Agent verts can be persistant, so free at end, not each frame*/
  agent_array_free(agent_array);
  agent_verts_free(agent_verts_new);
  free(framebuffer);
  system("pkill ffplay");
  printf("KILLED\n");
  glfwTerminate();
  return 0;
}
