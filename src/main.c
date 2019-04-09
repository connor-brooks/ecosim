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
float zoom = 1.0;
float xOffset = 0.0f;
float yOffset = 0.0f;

/* For passing pointers to callbacks */
struct User_ptrs{
  Agent_array* aa;
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
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    /* Put this in function*/
    struct User_ptrs* user_ptrs;
    Agent_array* aa;
    double xPos, yPos;
    int width, height;
    float xRel, yRel;
    Agent* tmp_agent;

    user_ptrs = glfwGetWindowUserPointer(window);
    aa = user_ptrs->aa;

    glfwGetCursorPos(window, &xPos, &yPos);
    glfwGetWindowSize(window, &width, &height);

    xRel = xPos / (float) width;
    yRel = yPos / (float) height;
    xRel *= 2;
    yRel *= 2;
    xRel -= 1;
    yRel -= 1;
    yRel = -yRel;
    xRel = (xRel - xOffset * zoom) / zoom;
    yRel = (yRel - yOffset * zoom) / zoom;

    tmp_agent = agent_create_random();
    tmp_agent->x = xRel;
    tmp_agent->y = yRel;
    agent_array_insert(aa, tmp_agent);

    printf("Inserted @ x %f, y %f\n", xRel, yRel);
  }

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  /* Put this in function*/
  int key = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
  float max;
  xoffset *= 0.025;
  yoffset *= 0.025;
  if(key){
    zoom += yoffset;

    zoom = MAX(1.0, zoom);
    zoom = MIN(2.0, zoom);

    /* keep in window */
    max = (1 / zoom) - 1;
    xOffset = MAX(max, xOffset);
    xOffset = MIN(-max, xOffset);
    yOffset = MAX(max, yOffset);
    yOffset = MIN(-max, yOffset);

    printf("Zoom @ %f\n", zoom);

  } else {
    xOffset += xoffset;
    yOffset += -yoffset;

    /* keep in window */
    max = (1 / zoom) - 1;
    xOffset = MAX(max, xOffset);
    xOffset = MIN(-max, xOffset);
    yOffset = MAX(max, yOffset);
    yOffset = MIN(-max, yOffset);
    printf("Scroll @ x %f, y %f\n", xOffset, yOffset);
  }
}

int
main(int argc, char **argv)
{
  GLFWwindow* window;
  Agent_array* agent_array;
  Agent_verts* agent_verts_new;
  Quadtree* quad;
  Quadtree_verts* quad_verts;
  int cyclecount = 0;
  float quad_head_pos[] = {-1.0f, -1.0f};
  float quad_head_size = 2.0f;
  // For passing structs between callbacks in glfw
  struct User_ptrs user_ptrs;

  /* Initalize glfw and glut*/
  if (!glfwInit())
    return -1; //exit

  /* Create window */
  window = glfwCreateWindow(300, 300, "ecosim", NULL, NULL);
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

  /* Setup shaders, agents and verts */
  agent_array = agent_array_setup_random(DEV_AGENT_COUNT);
  agents_insert_dead(agent_array, 10);
  agent_verts_new = agent_verts_create();
  GLuint agent_shader = gfx_agent_shader();
  GLuint agent_vis_shader = gfx_agent_vis_shader();
  GLuint world_shader = gfx_world_shader();

  /* Setup callbacks */
  user_ptrs.aa = agent_array;
  glfwSetWindowUserPointer(window, &user_ptrs);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);


  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


  // generate texture
  unsigned int texColorBuffer;
  glGenTextures(1, &texColorBuffer);
  glBindTexture(GL_TEXTURE_2D, texColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLuint test_shader = gfx_test_shader();



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


    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // we're not using the stencil buffer now
//    glViewport(0, 0, width, height);

    /* Main drawing */
    glPushMatrix();
    glScalef(zoom, zoom, 1.);
    glTranslatef(xOffset, yOffset, 1.0);

    /* Every frame rebuild the quadtree verts, draw them free them
     * will sort out later */
    quad_verts = quadtree_verts_create();
    quadtree_to_verts(quad, quad_verts);
    //gfx_quad_draw(quad_verts);
    quadtree_verts_free(quad_verts);

    /* Convert agents to verts and draw them
     * This function should only rebuild the verts array IF the agent count has changed,
     * so we free at end, as the struct should presist all through running of program*/
    agents_to_verts(agent_array, agent_verts_new);

    gfx_world_texture(world_shader, glfwGetTime());
    gfx_agents_draw_new(agent_verts_new, agent_shader, scale, zoom);
    gfx_agents_draw_vis(agent_verts_new, agent_vis_shader, scale, zoom);
    glPopMatrix();

    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glColor3f(1, 0, 0);

    glUseProgram(test_shader);
    glBegin(GL_QUADS);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glEnd();

    glUseProgram(0);

    /* Update */
    if(game_run)
    {
      agents_update(agent_array, quad);
      /* insert food agents every 100 cycles */
      if(cyclecount % 100 == 0) {
        agents_insert_dead(agent_array, 5);
        printf("ok\n");
        agent_array = agent_array_prune(agent_array);
        user_ptrs.aa = agent_array;
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
  glfwTerminate();
  return 0;
}
