#include "Sound.h"
#include "gltron.h"

#define NUM_GAME_FX 3

static char *game_fx_names[] = {

  /* #ifdef WIN32 || macintosh */
#if 1
  "game_engine.wav",
  "game_crash.wav",
  "game_recognizer.wav"
#else
  "game_engine.ogg",
  "game_crash.ogg",
  "game_recognizer.ogg"
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
      fprintf(stderr, "[error] can't load sound fx file %s\n",
	     game_fx_names[i]);
      exit(1); // FIXME: handle missing fx somewhere else
    }
  }
}

void reloadTrack() {
  char *song;
  char *path;
  scripting_GetStringSetting("current_track", &song);
  fprintf(stderr, "loading song %s\n", song);
  path = getPath( PATH_MUSIC, song );
  free(song);
  if(path == NULL) {
    fprintf(stderr, "can't find song...exiting\n");
    exit(1); // FIXME: handle missing song somewhere else
  }
  loadSound(path);
  playSound();

  free(path);
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
  const char *music_path;
  list *soundList;
  list *p;
  int i;

  music_path = getDirectory( PATH_MUSIC );
  soundList = readDirectoryContents(music_path, SONG_PREFIX);
  if(soundList->next == NULL) {
    fprintf(stderr, "no music files found...exiting\n");
    exit(1); // FIXME: handle missing songs somewhere else
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

  Audio_Init();
  loadFX();
  Audio_LoadPlayers();
  setFxVolume(getSettingf("fxVolume"));
  reloadTrack();
  setMusicVolume(getSettingf("musicVolume"));
  Audio_Start();
}
