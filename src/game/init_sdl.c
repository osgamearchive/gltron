#include "game/init.h"

#include "SDL.h"
#include <stdlib.h>

int video_initialized = 0;

void audioInit(void) {
  if(SDL_Init(SDL_INIT_AUDIO) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL audio: %s\n", SDL_GetError());
    /* FIXME: disable sound system */
  }
}

void videoInit(void) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Couldn't initialize SDL video: %s\n", SDL_GetError());
    exit(1); /* OK: critical, no visual */
  }
  else video_initialized = 1;
}

void inputInit(void) {
	/* keyboard */
  SDL_EnableKeyRepeat(0, 0); /* turn keyrepeat off */
  
	/* joystick */
	if(SDL_Init(SDL_INIT_JOYSTICK) >= 0) {
		int i;
		SDL_Joystick *joy;
		int joysticks = SDL_NumJoysticks();

		/* FIXME: why only two joysticks? */
		/* joystick, currently at most 2 */
		if(joysticks > 2)
			joysticks = 2;
		
		for(i = 0; i < joysticks; i++) {
			joy = SDL_JoystickOpen(i);
		}
		if(i)
			SDL_JoystickEventState(SDL_ENABLE);
	} else {
		const char *s = SDL_GetError();
		fprintf(stderr, "[init] couldn't initialize joysticks: %s\n", s);
	}
}
