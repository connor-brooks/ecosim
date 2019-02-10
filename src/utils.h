#ifndef UTILS_H
#define UTILS_H

typedef struct {
  float x;
  float y;
} Point;

typedef struct {
  Point p1;
  Point p2;
} Rectangle;

typedef struct {
  int red;
  int green;
  int blue;
} RGB;

Rectangle* createRectangle(float x1, float y1, float x2, float y2);

RGB* createRGB(float red, float green, float blue); 

#endif
