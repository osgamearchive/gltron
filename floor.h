#ifndef FLOOR_H
#define FLOOR_H

#include "GL/gl.h"

int drawFloorTextured(int grid_size, GLuint texture);
void drawFloorGrid(int grid_size,  int line_spacing,
                   float line_color[4], float square_color[4]);

#endif
