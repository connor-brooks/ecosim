#include <stddef.h>
#ifndef QUADTREE_H
#define QUADTREE_H
#include "config.h"

typedef struct Quadtree_ Quadtree;
typedef struct Quadtree_query_ Quadtree_query;

struct Quadtree_ {
  Quadtree* sect[QUAD_COUNT]; //see quadtree_sects
  float pos[QUADTREE_DIMS];
  float size;
  void* ptrs[QUADTREE_MAX_PER_CELL];
  size_t ptr_count;
  int has_child;
};

struct Quadtree_query_ {
  void** ptrs;
  size_t ptr_count;
  size_t capacity;
  size_t size;
};

Quadtree* quadtree_create(float pos[], float size);
void quadtree_free(Quadtree* q);
void quadtree_insert(Quadtree* q, void* ptr, float pos[]);
void quadtree_split(Quadtree *q);

Quadtree_query* quadtree_query_setup();
void quadtree_query_free(Quadtree_query* qq);
void quadtree_query(Quadtree *q, Quadtree_query* query, 
    float pos[], float size);
int quadtree_intersect(Quadtree *q, float pos[], float size);
void quadtree_query_add_ptr(Quadtree *quad, Quadtree_query* query);
void quadtree_query_dump(Quadtree_query* qq);

#endif
