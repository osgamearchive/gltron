#ifndef CALLBACKS_H
#define CALLBACKS_H

typedef struct Callbacks {
  void (*display)(void);
  void (*idle)(void);
  void (*keyboard)(int, int, int);
  void (*init)(void);
  void (*exit)(void);
  void (*initGL)(void);
  void (*mouse)(int, int, int, int);
  void (*mouseMotion)(int, int);
	char *name;
} Callbacks;

#endif
