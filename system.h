#ifndef SYSTEM_H
#define SYSTEM_H

#include "gltron.h"

#include <SDL.h>
#include <SDL_types.h>

#define SYSTEM_KEY_DOWN SDLK_DOWN
#define SYSTEM_KEY_UP SDLK_UP
#define SYSTEM_KEY_LEFT SDLK_LEFT
#define SYSTEM_KEY_RIGHT SDLK_RIGHT
#define SYSTEM_KEY_F1 SDLK_F1
#define SYSTEM_KEY_F2 SDLK_F2
#define SYSTEM_KEY_F3 SDLK_F3
#define SYSTEM_KEY_F4 SDLK_F4
#define SYSTEM_KEY_F5 SDLK_F5
#define SYSTEM_KEY_F6 SDLK_F6
#define SYSTEM_KEY_F7 SDLK_F7
#define SYSTEM_KEY_F10 SDLK_F10
#define SYSTEM_KEY_F11 SDLK_F11
#define SYSTEM_KEY_F12 SDLK_F12

#define SYSTEM_ENTER SDLK_ENTER
#define SYSTEM_RETURN SDLK_RETURN

#define SYSTEM_MOUSEUP SDL_MOUSEBUTTONUP
#define SYSTEM_MOUSEDOWN SDL_MOUSEBUTTONDOWN

#define SYSTEM_MOUSEPRESSED SDL_PRESSED
#define SYSTEM_MOUSERELEASED SDL_RELEASED

#define SYSTEM_MOUSEBUTTON_LEFT SDL_BUTTON_LEFT
#define SYSTEM_MOUSEBUTTON_RIGHT SDL_BUTTON_RIGHT

#define SYSTEM_KEY_TAB SDLK_TAB

#define SYSTEM_RGBA 1
#define SYSTEM_DOUBLE 2
#define SYSTEM_DEPTH 4
#define SYSTEM_FULLSCREEN 8
#define SYSTEM_32_BIT 16

/* system specific functions (basically, an SDL/glut wrapper) */

extern void SystemInit(int *argc, char *argv[]);
extern void SystemExit();
extern void SystemPostRedisplay();
extern int SystemGetElapsedTime();
extern void SystemSwapBuffers();
extern void SystemWarpPointer(int x, int y);
extern void SystemHidePointer();
extern void SystemUnhidePointer();
extern void SystemGrabInput();
extern void SystemUngrabInput();
extern void SystemMainLoop();
extern void SystemRegisterCallbacks(callbacks*);
extern void SystemSetGamma(float r, float g, float b);
extern void SystemInitWindow(int x, int y, int w, int h);
extern void SystemInitDisplayMode(int flags, unsigned char fullscreen);
extern int SystemCreateWindow(char *name);
extern void SystemDestroyWindow(int id);
extern void SystemReshapeFunc(void(*reshape)(int, int));

extern char* SystemGetKeyName(int key);

extern void SystemMouse(int buttons, int state, int x, int y);
extern void SystemMouseMotion(int x, int y);

extern int SystemWriteBMP(char *filename, int x, int y, unsigned char *pixels);

extern void SystemQuit();
#endif
