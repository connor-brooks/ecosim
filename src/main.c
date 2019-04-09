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
//#include "keyboard.h"
//#include "ui.h"
#include "quadtree.h"

#define DEV_AGENT_COUNT (80)


/* TEMPORARY GLOBAL */
int game_run = 1;
float zoom = 1.0;
float xOffset = 0.0f;
float yOffset = 0.0f;

/* For passing structs between main and callbacks, using glfw's
 * glfwGetWindowUserPointer(); function, as there is no way to pass
 * arguments to them */
struct User_ptrs{
  Agent_array* aa;
};

/* Keyboard callback */
void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  struct User_ptrs* user_ptrs;


  if(action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    //glfwSetWindowShouldClose(window, GLFW_TRUE);
    if(key == GLFW_KEY_SPACE)
    game_run = !game_run;
    /* For pointer passing between callback and main */
   // user_ptrs = glfwGetWindowUserPointer(window);
   // uig = user_ptrs->uig;
   // ui = user_ptrs->ui;

   // /* change to
   //  * ui_update = ui_get_key_resp(); */
   // /* Encode the key event to k_event struct, get response from UI */
   // k_event = keyboard_enc_event(key, mods);
   // ui_resp = ui_get_resp(ui, k_event);
   // /* ^ should stay in normal mode, unless CMD_WAIT_SEL from cmd is set,
   //  * if so, go into select mode. if select sucessful with ENTER, return UI_RESP_SELECTION
   //  * if no selection, just keep drawing as usual
   //  * if ESC, send UI_RESP_EXIT_CMD */
   // ui_gfx_update(ui_resp, uig);
   // /* ^ should be changed to take a ui_resp, instead of the whole ui struct */

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
    //free(k_event);
    //free(ui_resp);
  }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    struct User_ptrs* user_ptrs;
    double xPos, yPos;
    int width, height;
    float xRel, yRel;
    Agent* tmp_agent;
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

    printf("x %f, y %f\n", xRel, yRel);

    user_ptrs = glfwGetWindowUserPointer(window);
    Agent_array* aa = user_ptrs->aa;

    tmp_agent = agent_create_random();
    tmp_agent->x = xRel;
    tmp_agent->y = yRel;
    agent_array_insert(aa, tmp_agent);
    printf("S\n");
  }

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  xoffset *= 0.025;
  yoffset *= 0.025;
  int key = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
//  printf("offset x: %f, y %f\n", xoffset, yoffset);
  if(key){
    zoom += yoffset;
 //   printf("key %d\n", key);

    zoom = MAX(1.0, zoom);
    zoom = MIN(2.0, zoom);
    printf("zoom %f\n", zoom);

    /* keep in window */
    float max = (1 / zoom) - 1;

    xOffset = MAX(max, xOffset);
    xOffset = MIN(-max, xOffset);

    yOffset = MAX(max, yOffset);
    yOffset = MIN(-max, yOffset);

  }
  else {
    xOffset += xoffset;
    yOffset += -yoffset;
    
    /* keep in window */
    float max = (1 / zoom) - 1;

    xOffset = MAX(max, xOffset);
    xOffset = MIN(-max, xOffset);

    yOffset = MAX(max, yOffset);
    yOffset = MIN(-max, yOffset);

    printf("set is x %f, y %f\n", xOffset, yOffset);

  }
}

int
main(int argc, char **argv)
{
  GLFWwindow* window;

  Agent_array* agent_array;
  Agent_verts* agent_verts_new;
//  Agent_vis_verts* agent_vis_verts;
  int cyclecount = 0;
  /* Quadtree head pos info */
  float quad_head_pos[] = {-1.0f, -1.0f};
  float quad_head_size = 2.0f;

  Quadtree* quad;
  Quadtree_verts* quad_verts;

  //Ui* ui;
  //Ui_graphics* ui_gfx;


  // For passing structs between callbacks in glfw
  struct User_ptrs user_ptrs;

  /* Initalize glfw and glut*/
  glutInit(&argc, argv);
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
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  /* initalize glew and do various gl setup */
  glewInit();
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  srand((unsigned int)time(NULL));

  /* Setup agent arrary
   * setup agent verts
   * setup agent shader */
  agent_array = agent_array_setup_random(DEV_AGENT_COUNT);

  agents_insert_dead(agent_array, 10);

  agent_verts_new = agent_verts_create();
//  agent_vis_verts = agent_vis_verts_create();

  GLuint agent_shader = gfx_agent_shader();
  GLuint agent_vis_shader = gfx_agent_vis_shader();

  GLuint world_shader = gfx_world_shader();


  /* Setup UI graphics */
//  ui_gfx = ui_gfx_setup();

  /* keyboard test */
 // ui = ui_setup();

  /* This is to pass needed structs between glfw keyboard callback function and
   * main function, as they cannot be passed via arguments.
   * It's pretty hacky but an okay workaround*/
  //user_ptrs.ui = ui;
 // user_ptrs.uig = ui_gfx;
  user_ptrs.aa = agent_array;
  glfwSetWindowUserPointer(window, &user_ptrs);

  /* Main loop */
  while(!glfwWindowShouldClose(window))
  {
    /* Set the viewport */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    float scale = gfx_get_scale(window);

    /* Clear*/
    glClear(GL_COLOR_BUFFER_BIT);

    /* Recreate quadtree and insert agents */
    quad = quadtree_create(quad_head_pos, quad_head_size);

    for(int i = 0; i < agent_array->count; i++) {
      Agent* tmp_ptr = (agent_array->agents[i]);
      float tmp_pos[] = {tmp_ptr->x, tmp_ptr->y};
      if(tmp_ptr->state != AGENT_STATE_PRUNE)
        quadtree_insert(quad, tmp_ptr, tmp_pos);
    }
    glPushMatrix();
    glScalef(zoom, zoom, 1.);
    glTranslatef(xOffset, yOffset, 1.0);

    /* Every frame rebuild the quadtree verts, draw them free them
     * will sort out later */
    quad_verts = quadtree_verts_create();
    quadtree_to_verts(quad, quad_verts);
    gfx_quad_draw(quad_verts);
    quadtree_verts_free(quad_verts);

    /* Convert agents to verts and draw them
     * This function should only rebuild the verts array IF the agent count has changed,
     * so we free at end, as the struct should presist all through running of program*/
    agents_to_verts(agent_array, agent_verts_new);

    gfx_world_texture(world_shader, glfwGetTime());
    gfx_agents_draw_new(agent_verts_new, agent_shader, scale, zoom);
    gfx_agents_draw_vis(agent_verts_new, agent_vis_shader, scale, zoom);
    glPopMatrix();



    /* Draw UI */
    //ui_draw(ui_gfx);

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
  //ui_gfx_free(ui_gfx);
 // ui_free(ui);
  glfwTerminate();
  return 0;
}

