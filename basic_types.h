#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

typedef struct List List;
struct List {
  void *data;
  List* next;
};

typedef struct {
  float x, y;
} Point;
  
typedef struct {
  float sx, sy, ex, ey;
} Line;

typedef struct {
  unsigned int current;
  unsigned int lastFrame;
  unsigned int offset; /* from SystemGetElapsedTime() */
  unsigned int dt; /* current - lastFrame */

  /* float timeScale; */
} SystemTime;

#endif
