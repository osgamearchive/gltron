#ifndef Sound_H
#define Sound_H

#include "SDL.h"
#include "SDL_sound.h"

#include <zlib.h>
#include <assert.h>

extern void loadFX();
extern int  initSound();
extern void shutdownSound();
extern void loadSound(char *name);
extern void playSound();
extern void stopSound();
extern void soundIdle();
extern void setMusicVolume(float volume);
extern void setFxVolume(float volume);

#endif
