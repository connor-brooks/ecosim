#ifndef UTILS_H
#define UTILS_H

typedef struct {
  float x;
  float y;
} Point;

typedef struct Rectangle_ {
  Point p1;
  Point p2;
} Rectangle;

typedef struct RGB_ {
  int r;
  int g;
  int b;
} RGB;

Rectangle* create_rectangle(float x1, float y1, float x2, float y2);

RGB* create_RGB(float r, float g, float b); 

#endif
