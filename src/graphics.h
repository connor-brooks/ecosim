#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct Rectangle_ Rectangle;
typedef struct RGB_ RGB;

void draw_rectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void draw_text(float x, float y, const char* txt);
void draw_point(float x, float y, float r, float g, float b);
void draw_rectangle_struct(Rectangle* rect, RGB* c);
#endif
