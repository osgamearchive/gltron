#include "sound.h"

/* linux only, at the moment */

static int engine_channel = -1;
static Mix_Music *music;

#define NUM_GAME_FX 5
#define NUM_MENU_FX 2

static Mix_Chunk *game_fx[NUM_GAME_FX];
static Mix_Chunk *menu_fx[NUM_MENU_FX];

static char *fx_dir = "sounds";

static char *game_fx_names[] = {
  "game_engine.wav",
  "game_start.wav",
  "game_crash.wav",
  "game_win.wav",
  "game_lose.wav"
};

static char *menu_fx_names[] = {
  "menu_action.wav",
  "menu_highlight.wav"
};

void loadFX() {
  int i;
  char buf[100];
  for(i = 0; i < NUM_GAME_FX; i++) {
    sprintf(buf, "%s/%s", fx_dir, game_fx_names[i]);
    game_fx[i] = Mix_LoadWAV(buf);
  }
  for(i = 0; i < NUM_MENU_FX; i++) {
    sprintf(buf, "%s/%s", fx_dir, menu_fx_names[i]);
    menu_fx[i] = Mix_LoadWAV(buf);
  }
}
 
int initSound() {
  /* open the audio device */
  if(Mix_OpenAudio(22050, AUDIO_U16, 1, 1024) < 0) {
    fprintf(stderr, "can't open audio: %s\n", SDL_GetError());
    exit(2);
  }
  loadFX();
  return 0;
}


void shutdownSound() {
  Mix_CloseAudio();
}
  

int loadSound(char *name) {
  music = Mix_LoadMUS(name);
  return 0;
}

int playSound() {
  if( ! Mix_PlayingMusic() )
    Mix_PlayMusic(music, -1);
  return 0;
}

int stopSound() {
  if( Mix_PlayingMusic() )
    Mix_HaltMusic();
  return 0;
}

void soundIdle() {
  /* sdl_mixer uses pthreads, so no work here */
  return;
}

void playGameFX(int fx) {
  /* Mix_PlayChannel(-1, game_fx[fx], 0); */
  fprintf(stderr, "fx on channel %d\n", Mix_PlayChannel(-1, game_fx[fx], 0));
}

void playMenuFX(int fx) {
  Mix_PlayChannel(-1, menu_fx[fx], 0);
}

void playEngine() {
  if(engine_channel == -1) {
    engine_channel = Mix_PlayChannel(-1, game_fx[fx_engine], -1);
    fprintf(stderr, "started engine on channel %d\n", engine_channel);
  }
}

void stopEngine() {
  if(engine_channel != -1) {
    fprintf(stderr, "stopping engine on channel %d\n", engine_channel);
    Mix_HaltChannel(engine_channel);
    engine_channel = -1;
  }
}
