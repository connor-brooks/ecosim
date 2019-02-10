#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct Rectangle;
typedef struct RGB;

void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void drawText(float x, float y, const char* text);
void drawRectangleStruct(Rectangle* rectangle, RGB* color);
#endif
