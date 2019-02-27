#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

/* For passing structs between main and callbacks, using glfw's
 * glfwGetWindowUserPointer(); function, as there is no way to pass
 * arguments to them */
struct User_ptrs{
  Ui* ui;
  Ui_graphics* uig;
};

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
    /* For pointer passing between callback and main */
    user_ptrs = glfwGetWindowUserPointer(window);
    uig = user_ptrs->uig;
    ui = user_ptrs->ui;

    /* Encode the key event to k_event struct, get response from UI */
    k_event = keyboard_enc_event(key, mods);
    ui_resp = ui_get_resp(ui, k_event);

    /* Get keyboard response, and act upon it */
    //ui_get_resp(ui, k_event);
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
    free(k_event);
    free(ui_resp);
  }

}

/* Main function */

int
main(int argc, char **argv)
{
  int agent_count = 4;
  Agent agents[agent_count];
  float* agent_verts;
  Agent_graphics* agent_gfx;

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

  /* initalize glew*/
  glewInit();
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  //  glfwSetWindowUserPointer(window, keyboard);
  /* Setup agents */

  agents[0].x = 0.2;
  agents[0].y = 0.2;
  agents[0].rgb.r = 1.0;
  agents[0].rgb.g = 0.0;
  agents[0].rgb.b = 0.0;


  agents[1].x = 0.5;
  agents[1].y = 0.5;
  agents[1].rgb.r = 0.0;
  agents[1].rgb.g = 1.0;
  agents[1].rgb.b = 0.0;

  agents[2].x = 0.7;
  agents[2].y = 0.7;
  agents[2].rgb.r = 0.0;
  agents[2].rgb.g = 0.0;
  agents[2].rgb.b = 1.0;

  agents[3].x = 0.7;
  agents[3].y = 0.2;
  agents[3].rgb.r = 1.0;
  agents[3].rgb.g = 0.0;
  agents[3].rgb.b = 1.0;

  /* Create agent graphic vertex arrary */
  agent_verts = agents_to_vert(agents, agent_count, NULL, VERTS_NEW);


  /* Setup agent graphics */
  agent_gfx = agent_gfx_setup(agent_count, agent_verts, agents_vs, agents_fs);

  /* Maybe push this into setup soon too? */
  agent_vbo_setup(agent_gfx);
  agent_shader_setup(agent_gfx);

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

    gfx_agents_draw(agent_gfx);
    ui_draw(ui_gfx);

    glfwSwapBuffers(window);

    /* Poll for events */
    glfwPollEvents();

    // Hackily update the locations of the agents
    agents[0].x = sin(glfwGetTime());
    agents[0].y = 0.3 * sin( glfwGetTime());

    agents[1].x = sin(3 + glfwGetTime());
    agents[1].y = 0.8 * cos(3 + glfwGetTime());

    agents_to_vert(agents, agent_count, agent_verts, VERTS_UPDATE);

  }

  glfwTerminate();
  return 0;
}
