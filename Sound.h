#ifndef Sound_H
#define Sound_H

#include "gltron.h"

#include "SDL.h"
#include "SDL_sound.h"

#include <zlib.h>
#include <assert.h>

extern void Sound_loadFX();
extern void Sound_init();
extern void Sound_shutdown();
extern void Sound_load(char *name);
extern void Sound_play();
extern void Sound_stop();
extern void Sound_idle();
extern void Sound_setMusicVolume(float volume);
extern void Sound_setFxVolume(float volume);
extern void Sound_reloadTrack();
extern void Sound_initTracks();
extern void Sound_setup();
#endif
