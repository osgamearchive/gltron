#ifndef NEBU_SYSTEM_H
#define NEBU_SYSTEM_H

#include "nebu_callbacks.h"
#include "SDL.h"

/* system specific functions (basically, an SDL/glut wrapper) */
extern unsigned int SystemGetElapsedTime();

extern int SystemMainLoop();
extern void SystemExitLoop(int return_code);
extern void SystemRegisterCallbacks(Callbacks* callbacks);

extern void SystemExit();

extern void SystemHandleInput(SDL_Event *event);

extern Callbacks* current;

#endif
