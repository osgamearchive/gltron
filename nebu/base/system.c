#include "base/nebu_system.h"

#include "SDL.h"
#include <stdio.h>

Callbacks *current = 0;
static int return_code = -1;
static int redisplay = 0;
static int idle = 1;
static int fps_last = 0;
static int fps_dt = 1;

void SystemExit() {
	fprintf(stderr, "[system] shutting down SDL now\n");
	SDL_Quit();
	fprintf(stderr, "[system] scheduling application exit\n");

	/* TODO: ugly, please fix */
	redisplay = 0;
	idle = 0;
}

int nebu_Time_GetTimeForLastFrame()
{
	return fps_dt;
}

unsigned int nebu_Time_GetElapsed() {
	/* fprintf(stderr, "%d\n", SDL_GetTicks()); */
	return SDL_GetTicks();
}

static int lastFrame = 0;
void nebu_Time_SetCurrentFrameTime(unsigned t) {
	lastFrame = t;
}

unsigned int nebu_Time_GetElapsedSinceLastFrame() {
	return nebu_Time_GetElapsed() - lastFrame;
}

void nebu_Time_FrameDelay(unsigned int delay)
{
	if(nebu_Time_GetElapsedSinceLastFrame() < delay)
		nebu_System_Sleep(delay - nebu_Time_GetElapsedSinceLastFrame());
	// nebu_Time_SetCurrentFrameTime( nebu_Time_GetElapsed() );
}

int SystemMainLoop() {
	SDL_Event event;

	return_code = -1;
	while(return_code == -1) {
		while(SDL_PollEvent(&event) && current) {
			switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_JOYAXISMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION:
				SystemHandleInput(&event);
			break;
			case SDL_QUIT:
				SystemExit();
				SystemExitLoop(0);
			break;
			default:
				/* ignore event */
			break;
			}
		}
		if(redisplay) {
			current->display();
			redisplay = 0;
		}
		if(idle) 
			current->idle();
	}
	if(current->exit)
		(current->exit)();
	return return_code;
}
  
void nebu_System_SetCallbacks(Callbacks *cb) {
  current = cb;
	if(current && current->init)
		current->init();
}

void SystemExitLoop(int value) {
	return_code = value;
}

void SystemPostRedisplay() {
  redisplay = 1;
}

void nebu_System_SwapBuffers() {
	int now = nebu_Time_GetElapsed();
	fps_dt = now - fps_last;
	fps_last = now;
	nebu_Time_SetCurrentFrameTime(now);
	SDL_GL_SwapBuffers();
}

void nebu_System_SetCallback_Display( void(*display)(void) ) {
	current->display = display;
}

void nebu_System_SetCallback_Key( void(*keyboard)(int, int, int, int) ) {
	current->keyboard = keyboard;
}

void nebu_System_SetCallback_MouseMotion( void(*mouseMotion)(int, int) ) {
	current->mouseMotion = mouseMotion;
}

void nebu_System_SetCallback_Idle( void(*idle)(void) ) {
	current->idle = idle;
}

void nebu_System_Sleep(int ms) {
	SDL_Delay(ms);
}
