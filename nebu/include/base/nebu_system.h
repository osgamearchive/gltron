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

void nebu_System_SetCallback_Display( void(*display)(void) );
void nebu_System_SetCallback_MouseMove( void(*mouseMove)(int, int) );
void nebu_System_SetCallback_Idle( void(*idle)(void) );
void nebu_System_SetCallback_Key( void(*key)(int, int, int, int) );

unsigned int nebu_Time_GetElapsed();
unsigned int nebu_Time_GetElapsedSinceLastFrame();
void nebu_Time_SetCurrentFrameTime(unsigned t);

#endif
