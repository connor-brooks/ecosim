#ifndef UTILS_H
#define UTILS_H

typedef struct {
  float x;
  float y;
} Point;

typedef struct {
  Point topLeft;
  Point bottomRight;
} Rectangle;

typedef struct {
  int red;
  int green;
  int blue;
} RGB;

Rectangle* createRectangle(float x1, float y1, float x2, float y2);

RGB* createRGB(int red, int green, int blue); 

#endif
