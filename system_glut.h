#ifndef SYSTEM_H
#define SYSTEM_H

#include "gltron.h"

#ifdef FREEGLUT
#include "freeglut.h"
#else
#include <GL/glut.h>
/* #include <freeglut.h> */
#endif

/* system specific functions (basically, an SDL/glut wrapper) */

#define SYSTEM_KEY_DOWN GLUT_KEY_DOWN
#define SYSTEM_KEY_UP GLUT_KEY_UP
#define SYSTEM_KEY_LEFT GLUT_KEY_LEFT
#define SYSTEM_KEY_RIGHT GLUT_KEY_RIGHT
#define SYSTEM_KEY_F1 GLUT_KEY_F1
#define SYSTEM_KEY_F2 GLUT_KEY_F2
#define SYSTEM_KEY_F3 GLUT_KEY_F3
#define SYSTEM_KEY_F5 GLUT_KEY_F5
#define SYSTEM_KEY_F10 GLUT_KEY_F10
#define SYSTEM_KEY_F12 GLUT_KEY_F12

#define SYSTEM_RGBA GLUT_RGBA
#define SYSTEM_DOUBLE GLUT_DOUBLE
#define SYSTEM_DEPTH GLUT_DEPTH
#define SYSTEM_FULLSCREEN 0

extern void SystemInit(int *argc, char *argv[]);
extern void SystemPostRedisplay();
extern int SystemGetElapsedTime();
extern void SystemSwapBuffers();
extern void SystemWarpPointer(int x, int y);
extern void SystemMainLoop();
extern void SystemRegisterCallbacks(callbacks* cb);

extern void SystemInitWindow(int x, int y, int w, int h);
extern void SystemInitDisplayMode(int flags, unsigned char fullscreen);
extern int SystemCreateWindow(char *name);
extern void SystemDestroyWindow(int id);
extern void SystemReshapeFunc(void(*reshape)(int, int));

extern char* SystemGetKeyName(int key);
#endif


