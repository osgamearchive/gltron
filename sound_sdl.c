#include "sound.h"
#include "gltron.h"

/* linux only, at the moment */

static Mix_Music *music;
static int sound_open = 0;
#define SOUND_IS_OK if(!sound_open) return;
/*
#define NUM_GAME_FX 5
#define NUM_MENU_FX 2
*/
#define NUM_GAME_FX 2
#define NUM_MENU_FX 0

Mix_Chunk *game_fx[NUM_GAME_FX];
static Mix_Chunk *menu_fx[NUM_MENU_FX];

static char *game_fx_names[] = {
  "game_engine.wav",
  "game_crash.wav",

  "game_start.wav",
  "game_win.wav",
  "game_lose.wav"
};

static char *menu_fx_names[] = {
  "menu_action.wav",
  "menu_highlight.wav"
};

void loadFX() {
  int i;
  char *path;

  SOUND_IS_OK;

  for(i = 0; i < NUM_GAME_FX; i++) {
    path = getFullPath(game_fx_names[i]);
    if(path) {
      game_fx[i] = Mix_LoadWAV(path);
      free(path);
    } else {
      printf("error: can't load sound file %s\n",
	     game_fx_names[i]);
      exit(1);
    }
  }

  for(i = 0; i < NUM_MENU_FX; i++) {
    path = getFullPath(menu_fx_names[i]);
    if(path) {
      menu_fx[i] = Mix_LoadWAV(path);
      free(path);
    }
  }
}
 
int initSound() {
  /* open the audio device */
  if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) < 0) {
    fprintf(stderr, "can't open audio: %s\n", SDL_GetError());
    return 1;
  }
  sound_open = 1;

  loadFX();
  return 0;
}


void shutdownSound() {
  SOUND_IS_OK;
  Mix_CloseAudio();
}
  

void loadSound(char *name) {
  SOUND_IS_OK;
  music = Mix_LoadMUS(name);
  return;
}

void playSound() {
  SOUND_IS_OK;
  if( ! Mix_PlayingMusic() )
    Mix_PlayMusic(music, -1);
  /* todo: remove the following once the bug in SDL_mixer is fixed */
  /* we don't want too many references to game objects here */
  setMusicVolume(game->settings->musicVolume);
  return;
}

void stopSound() {
  SOUND_IS_OK;
  if( Mix_PlayingMusic() )
    Mix_HaltMusic();
  return;
}

void soundIdle() {
  /* sdl_mixer uses pthreads, so no work here */
  return;
}

int getFreeChannel() {
  int i;
  for(i = 0; i < MIX_CHANNELS; i++) {
    if(!Mix_Playing(i)) {
      return i;
    }
  }
  return -1;
}

void playGameFX(int fx, float volume) {
  int channel;
  SOUND_IS_OK;
  if((channel = getFreeChannel()) >= 0) {
       Mix_Volume(channel, volume * MIX_MAX_VOLUME);
       Mix_PlayChannel(channel, game_fx[fx], 0);
  }
}

void playMenuFX(int fx) {
  SOUND_IS_OK;
  Mix_PlayChannel(-1, menu_fx[fx], 0);
}

void setMusicVolume(float volume) {
  SOUND_IS_OK;
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;

  Mix_VolumeMusic((int)(volume * 128));
}

void setFxVolume(float volume) {
  SOUND_IS_OK;
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;

  Mix_Volume(-1, (int)(volume * 128));
}
