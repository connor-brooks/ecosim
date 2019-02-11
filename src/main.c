#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

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

	/* Program setup here */

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
//		float current = -0.8, end = 0.81;
//		for(; current <= end; current+=0.1)
//		drawText(-0.8, current,"ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM"); // High CPU, limit framerate
		draw_text(-0.8, 0.8,"ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM ECOSIM");

		/* Draw a rectangles via arguments */
		draw_rectangle(-0.6, -0.6, 0.6, 0.6, 0.0, 0.0, 0.0);
		draw_rectangle(0.0, 0.0, 0.3, -0.3, 0.0, 1.0, 0.0);
		draw_rectangle(0.0, 0.0, 0.3, 0.3, 1.0, 0.0, 1.0);
		draw_rectangle(0.0, 0.0, -0.3, 0.3, 0.0, 1.0, 1.0);
		draw_rectangle(0.0, 0.0, -0.3, -0.3, 1.0, 1.0, 0.0);


		/* Swap buffers */
		glfwSwapBuffers(window);

		/* Poll for events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
