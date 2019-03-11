#include "agents.h"
#ifndef QUADTREE_H
#define QUADTREE_H
#define QUAD_COUNT (4)
#define QUADTREE_MAX_PER_CELL (8)
#define QUADTREE_DIMS (2)

typedef struct Quadtree_ Quadtree;

struct Quadtree_ {
  Quadtree* sect[QUAD_COUNT]; //see quadtree_sects
  float pos[QUADTREE_DIMS];
  float size;
  void* ptrs[QUADTREE_MAX_PER_CELL];
  size_t ptr_count;
  int has_child;
};

Quadtree* quadtree_create(float pos[], float size);

void quadtree_insert(Quadtree* q, void* ptr, float pos[]);

void quadtree_split(Quadtree *q);

#endif
