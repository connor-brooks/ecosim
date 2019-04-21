#ifndef UTILS_H
#define UTILS_H
#define RANDF(x) (((float)rand()/(float)(RAND_MAX)) * x)
#define RANDF_MIN(min, max) ((((float) rand() / (float) RAND_MAX) * (max - min)) + min)
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


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
