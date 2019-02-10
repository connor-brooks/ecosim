#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "utils.h"
#include "graphics.h"

int
main(int argc, char **argv)
{
  GLFWwindow* window;

  /* Initalize glfw */
  if (!glfwInit())
    return -1; //exit

  /* Create window */
  window = glfwCreateWindow(640, 480, "ecosim", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Set the windows context */
  glfwMakeContextCurrent(window);


  /* Program setup */
  Rectangle* temp = createRectangle(-0.9, -0.9, 0.9, 0.9);
  RGB* color = createRGB(0, 0, 0);


  /* Main loop */
  while(!glfwWindowShouldClose(window))
  {
    /* Render */
    glClear(GL_COLOR_BUFFER_BIT);

    drawRectangle(temp, color);
    
    /* Swap buffers */
    glfwSwapBuffers(window);

    /* Poll for events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
