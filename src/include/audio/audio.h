#ifndef AUDIO_H
#define AUDIO_H

#include "sound_glue.h"

#define SONG_PREFIX "song_"

/* menu sound enumerations */
enum {
	fx_action,
	fx_highlight
};

#include "SDL.h"
#include "SDL_sound.h"

#include <zlib.h>
#include <assert.h>
#include <stdlib.h>

extern void Sound_loadFX(void);
extern void Sound_init(void);
extern void Sound_shutdown(void);
extern void Sound_load(char *name);
extern void Sound_play(void);
extern void Sound_stop(void);
extern void Sound_idle(void);
extern void Sound_setMusicVolume(float volume);
extern void Sound_setFxVolume(float volume);
extern void Sound_reloadTrack(void);
extern void Sound_initTracks(void);
extern void Sound_setup(void);

#endif
