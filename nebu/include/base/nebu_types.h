#ifndef NEBU_BASIC_TYPES_H
#define NEBU_BASIC_TYPES_H

typedef struct {
  unsigned int current;
  unsigned int lastFrame;
  unsigned int offset; /* from SystemGetElapsedTime() */
  unsigned int dt; /* current - lastFrame */

  /* float timeScale; */
} SystemTime;

#endif
