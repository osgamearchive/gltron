#include "base/nebu_system.h"
#include "base/nebu_argv.h"
#include "base/nebu_assert.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// review: 64bit ok

Callbacks *current = 0;
Callbacks default_callbacks;

static int return_code = -1;
static int redisplay = 0;
static int idle = 1;
static int fps_last = 0;
static int fps_dt = 1;

// TODO: move argv stuff to its own argv.c
nebu_argv* nebu_argv_Parse(int argc, const char *argv[])
{
	nebu_argv *pArguments = NULL;
	int nArgs = 0;
	int iArg = 0;
	int i;

	for(i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-' && argv[i][1] == '-' && strstr(argv[i], "=") != NULL)
			nArgs++;
	}
	pArguments = (nebu_argv*) malloc(sizeof(nebu_argv));
	if(nArgs == 0)
	{
		pArguments->nArguments = 0;
		pArguments->pKeys = NULL;
		pArguments->pValues = NULL;
		return pArguments;
	}
	pArguments->nArguments = nArgs;
	pArguments->pKeys = (char**) malloc(pArguments->nArguments * sizeof(char*));
	pArguments->pValues = (char**) malloc(pArguments->nArguments * sizeof(char*));

	for(i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-' && argv[i][1] == '-' && strstr(argv[i], "=") != NULL)
		{
			char* pEq = strstr(argv[i], "=");
#pragma GCC diagnostic ignored "-Wconversion"
			int keyLen = pEq - argv[i] - 2; // "--" subtracted
			int valLen = strlen(argv[i]) - keyLen - 3; // "--" and "=" subtracted 
#pragma GCC diagnostic warn "-Wconversion"
			pArguments->pKeys[iArg] = (char*) malloc((keyLen + 1) * sizeof(char));
			pArguments->pValues[iArg] = (char*) malloc((valLen + 1) * sizeof(char));
			strncpy(pArguments->pKeys[iArg], argv[i] + 2, keyLen);
			pArguments->pKeys[iArg][keyLen] = 0;
			strncpy(pArguments->pValues[iArg], argv[i] + 2 + keyLen + 1, valLen);
			pArguments->pValues[iArg][valLen] = 0;

			iArg++;
		}
	}
	return pArguments;
}

void nebu_argv_Debug_Print(nebu_argv *pArguments, FILE* output)
{
	int i;
	for(i = 0; i < pArguments->nArguments; i++)
	{
		fprintf(output, "'%s' ==> '%s'\n", pArguments->pKeys[i], pArguments->pValues[i]);
	}
}

void nebu_Init(void) {
	memset(&default_callbacks, 0, sizeof(Callbacks));
	current = &default_callbacks;
}

void nebu_System_Exit() {
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

void nebu_Time_Update()
{
	int now = nebu_Time_GetElapsed();
	fps_dt = now - fps_last;
	fps_last = now;
	nebu_Time_SetCurrentFrameTime(now);
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
	unsigned int timeSinceLastFrame = nebu_Time_GetElapsedSinceLastFrame();
	if(timeSinceLastFrame < delay)
		nebu_System_Sleep(delay - timeSinceLastFrame);
	// nebu_Time_SetCurrentFrameTime( nebu_Time_GetElapsed() );
}

#ifdef __IPHONEOS__

int nebu_HandleAppEvent(SDL_Event* event)
{
    switch(event->type)
    {
        case SDL_APP_TERMINATING:
            /* Terminate the app.
             Shut everything down before returning from this function.
             */
            return 0;
        case SDL_APP_LOWMEMORY:
            /* You will get this when your app is paused and iOS wants more memory.
             Release as much memory as possible.
             */
            return 0;
        case SDL_APP_WILLENTERBACKGROUND:
            /* Prepare your app to go into the background.  Stop loops, etc.
             This gets called when the user hits the home button, or gets a call.
             */
            return 0;
        case SDL_APP_DIDENTERBACKGROUND:
            /* This will get called if the user accepted whatever sent your app to the background.
             If the user got a phone call and canceled it, you'll instead get an    SDL_APP_DIDENTERFOREGROUND event and restart your loops.
             When you get this, you have 5 seconds to save all your state or the app will be terminated.
             Your app is NOT active at this point.
             */
            return 0;
        case SDL_APP_WILLENTERFOREGROUND:
            /* This call happens when your app is coming back to the foreground.
             Restore all your state here.
             */
            return 0;
        case SDL_APP_DIDENTERFOREGROUND:
            /* Restart your loops here.
             Your app is interactive and getting CPU again. */
            return 0;
        default:
            // not supposed to happen
            nebu_assert(0);
            return 0;
    }
}
#endif

int nebu_System_MainLoop() {
	SDL_Event event;

	return_code = -1;
	while(return_code == -1) {
		while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                case SDL_JOYAXISMOTION:
                case SDL_JOYBUTTONDOWN:
                case SDL_JOYBUTTONUP:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEMOTION:
                    nebu_Intern_HandleInput(&event);
                    break;
                case SDL_QUIT:
                    nebu_System_Exit(); // shut down
                    nebu_System_ExitLoop(0); // exit mainloop
                    break;
#ifdef __IPHONEOS__
                case SDL_APP_TERMINATING:
                case SDL_APP_LOWMEMORY:
                case SDL_APP_WILLENTERBACKGROUND:
                case SDL_APP_DIDENTERBACKGROUND:
                case SDL_APP_WILLENTERFOREGROUND:
                case SDL_APP_DIDENTERFOREGROUND:
                    nebu_HandleAppEvent(&event);
                    break;
#endif
                default:
                    /* ignore event */
                    break;
            }
		}
		if(current && current->display && redisplay) {
			current->display();
			redisplay = 0;
		}
		if(current && current->idle && idle) 
			current->idle();
	}
	if(current && current->exit)
		(current->exit)();
	current = NULL;
	return return_code;
}
  
void nebu_System_SetCallbacks(Callbacks *cb) {
	if(current && current->exit)
		(current->exit)();

	current = cb;
	if(current && current->init)
		current->init();
}

void nebu_System_ExitLoop(int value) {
	return_code = value;
}

void nebu_System_PostRedisplay() {
  redisplay = 1;
}

void nebu_System_SetCallback_Display( void(*display)(void) ) {
	current->display = display;
}

void nebu_System_SetCallback_Key( void(*keyboard)(int, int) ) {
	current->keyboard = keyboard;
}

void nebu_System_SetCallback_MouseMotion( void(*mouseMotion)(int, int) ) {
	current->mouseMotion = mouseMotion;
}

void nebu_System_SetCallback_Idle( void(*idle)(void) ) {
	current->idle = idle;
}

void nebu_System_Sleep(int ms) {
	nebu_assert(ms >= 0);
	// printf("sleeping for %d ms\n", ms);
	SDL_Delay(ms);
}
