#ifndef GRAPHICS_WORLD_H
#define GRAPHICS_WORLD_H

void drawWalls(void);
int drawFloorTextured(int grid_size, GLuint texture);
void drawFloorGrid(int grid_size,  int line_spacing,
                   float line_color[4], float square_color[4]);

#endif
