#include "Sound.h"
#include "gltron.h"

#include "SDL.h"
#include "SDL_sound.h"

/* linux only, at the moment */

#define NUM_GAME_FX 2

static char *game_fx_names[] = {

#ifdef macintosh 
  "game_engine.wav",
  "game_crash.wav"
#else
  "game_engine.ogg",
  "game_crash.ogg"
#endif
};

void loadFX() {
  int i;
  char *path;


  for(i = 0; i < NUM_GAME_FX; i++) {
    path = getPath(PATH_DATA, game_fx_names[i]);
    if(path) {
      Audio_LoadSample(path, i);
      free(path);
    } else {
      printf("error: can't load sound file %s\n",
	     game_fx_names[i]);
      exit(1);
    }
  }
}

reloadTrack() {
  char *song;
  char *path;
  scripting_GetString("current_track", &song);
  fprintf(stderr, "loading song %s\n", song);
  path = getPath( PATH_MUSIC, song );
  free(song);
  if(path == NULL) {
    fprintf(stderr, "can't find song...exiting\n");
  }
  loadSound(path);
  playSound();

  free(path);
}
 
int initSound() {
  Audio_Init();

  loadFX();
  return 0;
}


void shutdownSound() {
  Audio_Quit();
}
  

void loadSound(char *name) {
  Audio_LoadMusic(name);
}

void playSound() {
  Audio_SetMusicVolume(getSettingf("musicVolume"));
  Audio_PlayMusic();
  return;
}

void stopSound() {
  Audio_StopMusic();
}

void soundIdle() {
  Audio_Idle();
}

void setMusicVolume(float volume) {
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;
  Audio_SetMusicVolume(volume);
}

void setFxVolume(float volume) {
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;
  Audio_SetFxVolume(volume);
}

void initSoundTracks() {
  char *music_path;
  list *soundList;
  list *p;
  int i;

  music_path = getDirectory( PATH_MUSIC );
  soundList = readDirectoryContents(music_path, SONG_PREFIX);
  if(soundList->next == NULL) {
    fprintf(stderr, "no music files found...exiting\n");
    exit(1);
  }
    
  i = 1;
  for(p = soundList; p->next != NULL; p = p->next) {
    scripting_RunFormat("tracks[%d] = \"%s\"", i, (char*) p->data);
    i++;
  }
  scripting_Run("setupSoundTrack()");
}

void setupSound() {
  printf("initializing sound\n");
  initSound();
  setFxVolume(getSettingf("fxVolume"));
  reloadTrack();
  setMusicVolume(getSettingf("musicVolume"));
}
