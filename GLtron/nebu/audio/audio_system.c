#include "base/nebu_debug_memory.h"

#include "audio/nebu_audio_system.h"

#include "base/nebu_system.h"
#include <stdio.h>

void nebu_Audio_Init(void)
{
	if(SDL_Init(SDL_INIT_AUDIO) < 0 ){
		nebu_Log("Couldn't initialize SDL audio: %s\n", SDL_GetError());
		/* FIXME: disable sound system */
	}
}

