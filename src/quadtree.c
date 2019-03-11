#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quadtree.h"
#include "agents.h"


Quadtree*
quadtree_create(float pos[], float size)
{
  printf("Spanning %f %f to %f %f\n", pos[0], pos[1], pos[0]+size, pos[1]+size);

  int i;
  Quadtree* tmp = malloc(sizeof(Quadtree));

  /* Copy over dimentional info  and size*/
  memcpy(&tmp->pos, pos, sizeof(float) * QUADTREE_DIMS);
  memcpy(&tmp->size, &size, sizeof(float));

  /* NULL all sub-quad pointers */
  for(i = 0; i < QUAD_COUNT; i++) tmp->sect[i] = NULL;

  /* NULL all quad's pointers */
  for(i = 0; i < QUADTREE_MAX_PER_CELL; i++) tmp->ptrs[i] = NULL;

  tmp->ptr_count = 0;
  tmp->has_child = 0;

  printf("ok in here %f, %f size %f\n", tmp->pos[0], tmp->pos[1], tmp->size);
  return tmp;
}

void
quadtree_split(Quadtree *q)
{
  int i, j;
  int add_half;
  float child_size = q->size * 0.5f;
  float new_pos[QUAD_COUNT][QUADTREE_DIMS];
  printf("SPLITTING WITH SIZE %f\n", child_size);

  /* Loop through each new quad
   * Loop through each dimention
   * use anding n bitshifting to see if we should offset this new quads dims
   * Should create the new quads in clockwise manner... */
  for(i = 0; i < QUAD_COUNT; i++){
    for(j = 0; j < QUADTREE_DIMS; j++){
      add_half = ((1 << j) & i) ? 1 : 0;
      new_pos[i][j] = q->pos[j] + ((add_half) ? child_size : 0);
    }
  }
  for(i = 0; i < QUAD_COUNT; i++)
    q->sect[i] = quadtree_create(new_pos[i], child_size);

  q->has_child = 1;
}

void
quadtree_insert(Quadtree* q, void* ptr, float pos[])
{
  int i;
  /* ignore null ptr */
  if(ptr == NULL) return;
  /* go through dims, forget if outsifde */

  for(int i = 0; i < QUADTREE_DIMS; i++){
    if(pos[i] < q->pos[i])  return;
    if(pos[i] > q->pos[i] + q->size)  return;
  }

  if(q->ptr_count == QUADTREE_MAX_PER_CELL){
    printf("I AM FULL\n");
    // if not split, split quad
    if(!q->has_child) quadtree_split(q);
    // then just insert
    for(i = 0; i < QUAD_COUNT; i++)
      quadtree_insert(q->sect[i], ptr, pos);
  } else {
    printf("inserting 0x%x into %f, %f\n", ptr, pos[0], pos[1]);
    q->ptr_count++;
  }

}
