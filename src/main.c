#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include "agents.h"
#include "utils.h"
#include "graphics.h"

/* Keyboard callback */
void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if ( action == GLFW_PRESS)
    printf("key %c or %d with mod %d\n", key, key, mods);
  switch(key){
    case 81:
      glfwTerminate();
      break;
    default:
      break;
  };
}

/* Main function */

int
main(int argc, char **argv)
{
  GLFWwindow* window;

  float agents_data[] = {
    0.0f,    0.5f, 0.0f, 1.0f,
    0.5f, -0.366f, 0.0f, 1.0f,
    -0.5f, -0.366f, 0.0f, 1.0f,
    1.0f,    0.0f, 0.0f, 1.0f,
    0.0f,    1.0f, 0.0f, 1.0f,
    0.0f,    0.0f, 1.0f, 1.0f,
  };

  const char* agents_vs =
    "#version 130\n"
    "attribute vec4 position;"
    "attribute vec4 color;"
    "out vec4 color_out;"
    "void main() {"
    "color_out = color;"
    "  gl_Position = vec4(position);"
    "  gl_PointSize = 10.0;"
    "}";

  const char* agents_fs =
    "#version 130\n"
    "out vec4 frag_colour;"
    "in vec4 color_out;"
    "void main() {"
    " vec2 test2 = gl_PointCoord - vec2(0.5);"
    " if(length(test2) > 0.5) discard;"
    //"  gl_FragColor = color_out * (length(test2) + 0.3);"
    "  gl_FragColor = color_out ;"
    "}";


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


  /* Set up graphics for agents */
  Agent_graphics* agent_graphics = malloc(sizeof(Agent_graphics));
  agent_graphics->no_agents = 3;
  agent_graphics->vert_data = agents_data;
  agent_graphics->vert_data_len = sizeof(agents_data);
  agent_graphics->vert_shader = agents_vs;
  agent_graphics->frag_shader = agents_fs;

  agent_vbo_setup(agent_graphics);
  agent_shader_setup(agent_graphics);

  Agent agents[3];
  agents[0].x = 0.2;
  agents[0].y = 0.2;
  agents[1].x = 0.5;
  agents[1].y = 0.5;
  agents[2].x = 0.7;
  agents[2].y = 0.7;
  float* avs = agents_to_vert(agents, 3);
  int count = agent_vert_elems(3);
  for(int i = 0; i < count; i++)
  {
    printf("val %f...", avs[i]);
  }
  printf("\n");






  /* Main loop */
  while(!glfwWindowShouldClose(window))
  {
    /* Set the viewport */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    /* Render */
    glClear(GL_COLOR_BUFFER_BIT);


    /* TESTING GRAPHICS FUNCTIONS */

    /* Draw a rectangle from a struct */
    Rectangle* temp = create_rectangle(0.9, 0.9, -0.9, -0.9);
    RGB* color = create_RGB(1.0, 1.0, 1.0);
    draw_rectangle_struct(temp, color);
    free(temp);
    free(color);

    /* Text function test */
    draw_text(-0.8, 0.8,(const unsigned char *)"ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM");

    /* Draw a rectangles via arguments */
    draw_rectangle(-0.6, -0.6, 0.6, 0.6, 0.0, 0.0, 0.0);
    draw_rectangle(0.0, 0.0, 0.3, -0.3, 0.0, 1.0, 0.0);
    draw_rectangle(0.0, 0.0, 0.3, 0.3, 1.0, 0.0, 1.0);
    draw_rectangle(0.0, 0.0, -0.3, 0.3, 0.0, 1.0, 1.0);
    draw_rectangle(0.0, 0.0, -0.3, -0.3, 1.0, 1.0, 0.0);


    agents_draw(agent_graphics);
    /* Swap buffers */
    glfwSwapBuffers(window);

    /* Poll for events */
    glfwPollEvents();

    // Hackily update the locations of the agents
    agents_data[0] = sin(glfwGetTime());
    agents_data[1] = 0.3 * sin( glfwGetTime());

    agents_data[4] = sin(3 + glfwGetTime());
    agents_data[5] = 0.8 * cos(3 + glfwGetTime());

    agents_data[8] = sin(glfwGetTime());
  }

  glfwTerminate();
  return 0;
}
