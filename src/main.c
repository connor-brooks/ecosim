#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "utils.h"
#include "graphics.h"

int
main(int argc, char **argv)
{
  GLFWwindow* window;

  /* Initalize glfw */
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


  /* Program setup */
  Rectangle* temp = createRectangle(0.9, 0.9, -0.9, -0.9);
  RGB* color = createRGB(1.0, 1.0, 1.0);

  Rectangle* temp2 = createRectangle(0.0, 0.0, -0.3, -0.3);
  RGB* color2 = createRGB(1.0, 1.0, 0.0);

  Rectangle* temp3 = createRectangle(0.0, 0.0, -0.3, 0.3);
  RGB* color3 = createRGB(0.0, 1.0, 1.0);

  Rectangle* temp4 = createRectangle(0.0, 0.0, 0.3, 0.3);
  RGB* color4 = createRGB(1.0, 0.0, 1.0);

  Rectangle* temp5 = createRectangle(0.0, 0.0, 0.3, -0.3);
  RGB* color5 = createRGB(0.0, 1.0, 0.0);

  Rectangle* temp6 = createRectangle(-0.6, -0.6, 0.6, 0.6);
  RGB* color6 = createRGB(0.0, 0.0, 0.0);


  /* Main loop */
  while(!glfwWindowShouldClose(window))
  {
    /* Set the viewport */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    /* Render */
    glClear(GL_COLOR_BUFFER_BIT);

    drawRectangle(temp, color);


    drawRectangle(temp6, color6);

    drawRectangle(temp2, color2);
    drawRectangle(temp3, color3);
    drawRectangle(temp4, color4);
    drawRectangle(temp5, color5);


    drawText(-0.8, 0.8,"ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM");
    drawText(-0.8, -0.8,"ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM");

    /* Swap buffers */
    glfwSwapBuffers(window);

    /* Poll for events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
