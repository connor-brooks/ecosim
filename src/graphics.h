#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct Rectangle;
typedef struct RGB;

void draw_rectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void draw_text(float x, float y, const char* text);
void draw_point(float x, float y, float r, float g, float b);
void draw_rectangle_struct(Rectangle* rectangle, RGB* color);
#endif
