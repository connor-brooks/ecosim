#ifndef SHADERS_H
#define SHADERS_H

/* Agent shaders */
const char* agents_vs =
"#version 130\n"
"attribute vec4 position;"
"attribute vec4 color;"
"out vec4 color_out;"
"void main() {"
"color_out = color;"
"  gl_Position = vec4(position.x, position.y, position.z, 1.0);"
"  gl_PointSize = position.w;"
"}";

const char* agents_fs =
"#version 130\n"
"out vec4 frag_colour;"
"in vec4 color_out;"
"void main() {"
" vec2 test2 = gl_PointCoord - vec2(0.5);"
" if(length(test2) > 0.5) discard;"
//"  gl_FragColor = color_out * (length(test2) + 0.3);"
"  gl_FragColor = color_out ;"
"}";

#endif
