#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include "quadtree.h"



Quadtree*
quadtree_create(float pos[], float size)
{
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

  return tmp;
}

/* free a quad and all it's children */
void
quadtree_free(Quadtree* q)
{
  int i;
  if(q->has_child){
    for(i = 0; i < QUAD_COUNT; i++){
      quadtree_free(q->sect[i]);
    }
  }
  free(q);
}

void
quadtree_split(Quadtree *q)
{
  int i, j;
  int add_half;
  float child_size = q->size * 0.5f;
  float new_pos[QUAD_COUNT][QUADTREE_DIMS];

  // This is stupid do this in a standard way don't do this:
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

/* insert a pointer to the quadtree a pos */
void
quadtree_insert(Quadtree* q, void* ptr, float pos[])
{
  int i;
  ptrdiff_t search_pos;
  /* ignore null ptr */
  if(ptr == NULL) return;
  /* go through dims, forget if outsifde */

  /* exit if out of bounds */
  for(int i = 0; i < QUADTREE_DIMS; i++){
    if(pos[i] < q->pos[i])  return;
    if(pos[i] > q->pos[i] + q->size)  return;
  }

  /* if quad is full */
  if(q->ptr_count == QUADTREE_MAX_PER_CELL){
    // if not split, split quad
    if(!q->has_child) quadtree_split(q);
    // then insert into splits
    for(i = 0; i < QUAD_COUNT; i++)
      quadtree_insert(q->sect[i], ptr, pos);
  }
  /* if quad has space */
  else {
    /* find empty pos and insert*/
    search_pos = 0;
    while(q->ptrs[search_pos] != NULL) search_pos++;
    q->ptrs[search_pos] = ptr;
    q->ptr_count++;
  }
}



/* check if a point intersects with a quad */
int quadtree_intersect(Quadtree *q, float pos[], float size)
{
  float half_size = size * 0.5;

  if ((pos[0] - half_size <= q->pos[0] + q->size) &&
      (pos[1] - half_size <= q->pos[1] + q->size) &&
      (q->pos[0] <= pos[0] + half_size) &&
      (q->pos[1] <= pos[1] + half_size))
    return 1;

  else return 0;
}

Quadtree_query*
quadtree_query_setup()
{
  Quadtree_query* tmp = malloc(sizeof(Quadtree_query));

  tmp->capacity =  sizeof(void*) * QUADTREE_QUERY_SIZE;
  tmp->ptrs = malloc(tmp->capacity);
  tmp->size = 0;
  tmp->ptr_count = 0;

  return tmp;
}


void
quadtree_query_free(Quadtree_query* qq)
{
  free(qq->ptrs);
  free(qq);
}

void
quadtree_query(Quadtree *q, Quadtree_query* query, float pos[], float size)
{
  /* if no intersection ,ignore */
  if(!quadtree_intersect(q, pos, size)) return;

  /* If quad has children, recusivly go through and add them to query*/
  if(q->has_child){
    for(int i = 0; i < QUAD_COUNT; i++) {
      quadtree_query(q->sect[i], query, pos, size);
    }
  }
  /* add pointers to query from this quad */
  quadtree_query_add_ptr(q, query);
}

void
quadtree_query_add_ptr(Quadtree *quad, Quadtree_query* query)
{
  /* predict for the worst, the quad is full */
  size_t new_size = query->size + (sizeof(void*) * QUADTREE_MAX_PER_CELL);

  /* resize if needed */
  if(new_size > query->capacity){
    query->capacity *= 2;
    query->ptrs = realloc(query->ptrs, query->capacity);
  }

  for(int i = 0; i < QUADTREE_MAX_PER_CELL; i++) {
    if(quad->ptrs[i] != NULL) {
      /* Add the pointer to end of query pointer array */
      query->ptrs[query->ptr_count] = quad->ptrs[i];
      query->size += sizeof(void*);
      query->ptr_count += 1;
    } else
    {
      //printf("ERR: got null\n");
    }
  }
}

/* For debug */
void
quadtree_query_dump(Quadtree_query* qq)
{
  printf("DUMP: Got %d pointers\n", qq->ptr_count);
  for(int i =0; i < qq->ptr_count; i++)
    printf("DUMP: Got pointer 0x%x at loc %d\n", (void*) qq->ptrs[i], i);
}
