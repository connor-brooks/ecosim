#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include "agents.h"
#include "utils.h"
#include "graphics.h"
#include "shaders.h"
#include "keyboard.h"

/* Keyboard callback */
void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS){
    Keyboard* keyb = glfwGetWindowUserPointer(window);
    keyboard_action(keyb, key, mods);
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
  Keyboard* keyboard;

  GLFWwindow* window;

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

  glfwSetWindowUserPointer(window, keyboard);
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

  /* Set values 4 graphics 4 agents */
  agent_gfx = malloc(sizeof(Agent_graphics));
  agent_gfx->no_agents = agent_count;
  agent_gfx->vert_data = agent_verts;
  agent_gfx->vert_data_len = agent_vert_elems(agent_count) * sizeof(float);
  agent_gfx->vert_shader = agents_vs;
  agent_gfx->frag_shader = agents_fs;

  /* Setup based on this */
  agent_vbo_setup(agent_gfx);
  agent_shader_setup(agent_gfx);

  /* Setup UI graphics */
  ui_gfx = malloc(sizeof(Ui_graphics));
  ui_gfx_setup(ui_gfx);
  ui_gfx->cmd_txt = "hello world\0";

  /* keyboard test */
  keyboard = malloc(sizeof(Keyboard));
  keyboard_ui_ptr(keyboard, ui_gfx);
  glfwSetWindowUserPointer(window, keyboard);


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

    agents_draw(agent_gfx);
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
