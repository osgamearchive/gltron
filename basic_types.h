#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include "GL/gl.h"

typedef struct list list;
struct list {
  void *data;
  list* next;
};

typedef struct Point {
  float x, y;
} Point;
  
typedef struct line {
  int sx, sy, ex, ey;
} Line;

typedef struct {
  unsigned int current;
  unsigned int lastFrame;
  unsigned int offset; /* from SystemGetElapsedTime() */
  unsigned int dt; /* current - lastFrame */

  /* float timeScale; */
} SystemTime;

#endif
