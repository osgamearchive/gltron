#ifndef INPUT_H
#define INPUT_H

typedef struct {
  int mouse1;
  int mouse2;
  int mousex;
  int mousey;
} Input;


#define MOUSE_ORIG_X 100
#define MOUSE_ORIG_Y 100
#define MOUSE_CX 0.003
#define MOUSE_CY 0.003

extern void keyGame(int state, int key, int x, int y);

extern int joy_threshold;
extern Input gInput;
#endif
