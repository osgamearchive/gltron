#include "sound.h"

/* the following functions do nothing and are provided for
   compatibility only */

void playGameFX(int fx) { }
void playMenuFX(int fx) { }
void playEngine() { }
void stopEngine() { }
void shutdownSound() { }

/* here is the music stuff */

MODULE* sound_module;

int initSound() {
  char *drivers;
  md_mode |= DMODE_SOFT_MUSIC;
  md_mixfreq = 44100;
  md_reverb = 0;

#ifdef WIN32
  MikMod_RegisterDriver(&drv_win);
#else
  MikMod_RegisterAllDrivers();
#endif
  drivers = MikMod_InfoDriver();
  printf("%s\n", drivers);
  free(drivers);

  MikMod_RegisterAllLoaders();

  if(MikMod_Init("")) {
    printf("Cound not initialize sound: %s\n",
	   MikMod_strerror(MikMod_errno));
    return 1;
  }
  return 0;
}

void loadSound(char* name) {
  sound_module = Player_Load(name, 64, 0);
  if(!sound_module) {
    fprintf(stderr, "Could not load module: %s\n",
	   MikMod_strerror(MikMod_errno));
  }
}

void playSound() {
  if (sound_module) {
    Player_Start(sound_module);
    printf("sound startet\n");
  }
}

void stopSound() {
  Player_Stop();
  printf("sound stopped");
}

void deleteSound() {
  if(Player_Active())
    Player_Stop();
  if(sound_module)
    Player_Free(sound_module);
  MikMod_Exit();
}

void soundIdle() {
  if(Player_Active())
    MikMod_Update();
}





