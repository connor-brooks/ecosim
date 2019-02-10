#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct Rectangle;
typedef struct RGB;

void drawRectangle(Rectangle* rectangle, RGB* color);
void drawText(float x, float y, const char* text);
#endif
