#ifndef CALLBACKS_H
#define CALLBACKS_H
typedef struct callbacks {
  void (*display)(void);
  void (*idle)(void);
  void (*keyboard)(int, int, int, int);
  void (*init)(void);
  void (*exit)(void);
  void (*initGL)(void);
  void (*mouse)(int, int, int, int);
  void (*mouseMotion)(int, int);
} callbacks;
#endif
