#ifndef SYSTEM_H
#define SYSTEM_H

#include <SDL.h>
#include <SDL_net.h> 
#include <SDL_types.h>
extern void SystemInit(int *argc, char *argv[]);
extern int SystemGetElapsedTime();
#endif

