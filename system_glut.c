/* system specific functions (basically, an SDL wrapper) */

#include "system.h"

static int win_x, win_y;
static int width, height;
static int flags;
static unsigned char fullscreen;
static callbacks *current = 0;

void SystemInit(int *argc, char **argv) {
  glutInit(argc, argv);
}

void SystemPostRedisplay() {
  glutPostRedisplay();
}

int SystemGetElapsedTime() {
#ifdef WIN32
	return timeGetTime();
#else
  return glutGet(GLUT_ELAPSED_TIME);
#endif
}

void SystemSwapBuffers() {
  glutSwapBuffers();
}

void SystemWarpPointer(int x, int y) {
  glutWarpPointer(x, y);
}

void SystemMainLoop() {
  glutMainLoop();
}

void SystemMouse(int buttons, int state, int x, int y) {
  if(current)
    if(current->mouse != NULL)
      current->mouse(buttons, state, x, y);
}

void SystemMouseMotion(int x, int y) {
  if(current)
    if(current->mouseMotion != NULL)
      current->mouseMotion(x, y);
}

void SystemKeyboard(unsigned char key, int x, int y) {
  if(current)
    current->keyboard(key, x, y);
}

void SystemSpecial(int key, int x, int y) {
  if(current)
    current->keyboard(key, x, y);
}
  
void SystemRegisterCallbacks(callbacks *cb) {
  current = cb;
  glutIdleFunc(cb->idle);
  glutDisplayFunc(cb->display);
  glutKeyboardFunc(SystemKeyboard);
  glutSpecialFunc(SystemKeyboard);
  glutMouseFunc(SystemMouse);
  glutPassiveMotionFunc(SystemMouseMotion);
}

void SystemInitWindow(int x, int y, int w, int h) {
  win_x = x;
  win_y = y;
  width = w;
  height = h;
}

void SystemInitDisplayMode(int f, unsigned char full) {
  flags = f;
  fullscreen = full;
}

int SystemCreateWindow(char *name) {
  if(fullscreen) {
    // do glut game mode stuff
  } else {
    glutInitWindowPosition(win_x, win_y);  
    glutInitWindowSize(width, height);
    glutInitDisplayMode(flags);
    return glutCreateWindow(name);
  }
  return 0;
}

void SystemDestroyWindow(int id) {
  glutDestroyWindow(id);
}

void SystemReshapeFunc(void(*reshape)(int, int)) {
  glutReshapeFunc(reshape);
}

extern char* SystemGetKeyName(int key) {
  char *buf;

  buf = malloc(2);
  buf[0] = (char)key;
  buf[1] = 0;
  return buf;
}  
