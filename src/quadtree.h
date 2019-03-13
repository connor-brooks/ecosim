#include <stddef.h>
#include "agents.h"
#ifndef QUADTREE_H
#define QUADTREE_H
#define QUAD_COUNT (4)
#define QUADTREE_MAX_PER_CELL (4)
#define QUADTREE_DIMS (2)

typedef struct Quadtree_ Quadtree;
typedef struct Quadtree_verts_ Quadtree_verts;
typedef struct Quadtree_query_ Quadtree_query;

struct Quadtree_ {
  Quadtree* sect[QUAD_COUNT]; //see quadtree_sects
  float pos[QUADTREE_DIMS];
  float size;
  void* ptrs[QUADTREE_MAX_PER_CELL];
  size_t ptr_count;
  int has_child;
};

struct Quadtree_verts_ {
  float* verts; //vert data
  size_t capacity; //byte length
  size_t size; //byte length
  int q_count; // count of quads
  ptrdiff_t end; // end pointer
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

Quadtree_verts* quadtree_verts_create();
void quadtree_verts_free(Quadtree_verts* qv);
void quadtree_to_verts(Quadtree* q, Quadtree_verts* v);

Quadtree_query* quadtree_query_setup();
void quadtree_query(Quadtree *q, Quadtree_query* query, float pos[], float size);
int quadtree_intersect(Quadtree *q, float pos[], float size);
void quadtree_query_add_ptr(Quadtree *quad, Quadtree_query* query);

#endif
