#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include "agents.h"
#include "utils.h"
#include "graphics.h"
#include "shaders.h"
#include "keyboard.h"
#include "ui.h"
#include "quadtree.h"

#define DEV_AGENT_COUNT (300)

/* For passing structs between main and callbacks, using glfw's
 * glfwGetWindowUserPointer(); function, as there is no way to pass
 * arguments to them */
struct User_ptrs{
  Ui* ui;
  Ui_graphics* uig;
};

/* TEMPORARY GLOBAL */
int game_run = 1;

/* Keyboard callback */
void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  struct User_ptrs* user_ptrs;
  Ui_graphics* uig;
  Ui* ui;
  Ui_resp* ui_resp;
  Keyboard_event* k_event;


  if(action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    game_run = !game_run;
    /* For pointer passing between callback and main */
    user_ptrs = glfwGetWindowUserPointer(window);
    uig = user_ptrs->uig;
    ui = user_ptrs->ui;

    /* Encode the key event to k_event struct, get response from UI */
    k_event = keyboard_enc_event(key, mods);
    ui_resp = ui_get_resp(ui, k_event);
    /* ^ should stay in normal mode, unless CMD_WAIT_SEL from cmd is set,
     * if so, go into select mode. if select sucessful with ENTER, return UI_RESP_SELECTION
     * if no selection, just keep drawing as usual
     * if ESC, send UI_RESP_EXIT_CMD */
    ui_gfx_update(ui_resp, uig);
    /* ^ should be changed to take a ui_resp, instead of the whole ui struct */

    /* cmd_run(ui) // run cmd from ui
     * ^ can respond in two ways:
     * 1) sending the enum CMD_RAN_OKAY ( the command ran fine)
     * 2) storing cmd and waiting for select data send CMD_WAIT_SEL, only stop sending
     *    if UI_RESP_SELECTION is sent (sucesss) or UI_RESP_EXIT_CMD is sent
     *    (if exit is the case, drop all command buffer info and go into CMD_SEL_CANCEL
     *
     * ui_update_mode()
     * ^ update the mode as needed, set it to select mode if CMD_WAIT_SEL
     *   this will prompt the user to do a keypress, starting to selecting
     *   which is done via ui_get_resp()
     *   set it back to normal mode if CMD_SEL_CANCEL
     *   set it back to normal if CMD_RAN_OKAY */

    /* Bit of debug */
    //  printf("key %d or %c, special %d\n", k_event->ch, k_event->ch, k_event->special);
    free(k_event);
    free(ui_resp);
  }

}

/* Main function */

int
main(int argc, char **argv)
{
  srand((unsigned int)time(NULL));
  int agent_count;
  float* agent_verts;
  Agent_graphics* agent_gfx;
  Agent_array* agent_array;
  Quadtree* quad;

  Ui_graphics* ui_gfx;
  //Keyboard* keyboard;
  Ui* ui;

  GLFWwindow* window;

  // For passing structs between callbacks in glfw
  struct User_ptrs user_ptrs;

  /* Initalize glfw and glut*/
  if (!glfwInit())
    return -1; //exit
  glutInit(&argc, argv);


  /* Create window */
  window = glfwCreateWindow(300, 300, "ecosim", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Set the windows context */
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  /* initalize glew and do various gl setup */
  glewInit();
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  // /* new setup agents */
  agent_count = DEV_AGENT_COUNT;
  agent_array = agent_array_setup(agent_count);

  /* Create agent graphic vertex arrary */
  agent_verts = agents_to_vert(agent_array->agents, agent_array->count, NULL, VERTS_NEW);

  /* Setup agent graphics */
  agent_gfx = agent_gfx_setup(agent_array->count, agent_verts, agents_vs, agents_fs);

  /* Maybe push this into setup soon too? */
  agent_vbo_setup(agent_gfx);
  agent_shader_setup(agent_gfx);

  /* Setup quadtree */
  float quadRootPos[] = {-1.0, -1.0};
  quad = quadtree_create(quadRootPos, 2.0);

  for(int i = 0; i < agent_array->count; i++) {
    Agent* tmp_ptr = &agent_array->agents[i];
    float tmp_pos[] = {tmp_ptr->x, tmp_ptr->y};
    quadtree_insert(quad, tmp_ptr, tmp_pos);
  }


  /* Setup UI graphics */
  ui_gfx = ui_gfx_setup();

  /* keyboard test */
  ui = ui_setup();

  /* This is to pass needed structs between glfw keyboard callback function and
   * main function, as they cannot be passed via arguments.
   * It's pretty hacky but an okay workaround*/
  user_ptrs.ui = ui;
  user_ptrs.uig = ui_gfx;
  glfwSetWindowUserPointer(window, &user_ptrs);


  /* Main loop */
  while(!glfwWindowShouldClose(window))
  {
    /* Set the viewport */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    /* Render */
    glClear(GL_COLOR_BUFFER_BIT);
    /* Text function test */
    //printf("trying to draw\n");
    gfx_agents_draw(agent_gfx);
    ui_draw(ui_gfx);
    //printf("trying to draw ui\n");

    /*
     *  cmd_run(agent_array); // if count change, toggle count_change
     *  agent_update(agent_array); // if count change, toggle count_change
     *  agent_gfx_regen_vbo(agent_gfx, agent_array) // if count change, regen the vbo
     *  agents_to_verts(agent_array, verts) // if count change, regen completly
     *  agents_null_count_change(agents_array); // all count change done, reset the flag
     *  agents_gfx_draw_agents(array);
     */


    // Update the positional data and VBO
    if(game_run)
    {
      agents_update(agent_array);
      agent_vbo_update(agent_gfx, agent_array);
    }

    /* swap */
    glfwSwapBuffers(window);
    /* Poll for events */
    glfwPollEvents();

  }

  glfwTerminate();
  return 0;
}

