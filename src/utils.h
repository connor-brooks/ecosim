#ifndef UTILS_H
#define UTILS_H
#define RANDF(x) (((float)rand()/(float)(RAND_MAX)) * x)

typedef struct {
  float x;
  float y;
} Point;

typedef struct RGB_ {
  float r;
  float g;
  float b;
} RGB;


#endif
