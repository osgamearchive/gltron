#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

int initSound();
void shutdownSound();

void loadSound(char *name);
void playSound();
void stopSound();
void deleteSound();
void soundIdle();

void setMusicVolume(float volume);
void setFxVolume(float volume);

void playGameFX(int fx, float volume);
void playMenuFX(int fx);

void playEngine();
void stopEngine();

enum game_fx { fx_engine=0, fx_crash, fx_start, fx_win, fx_lose };

enum { 
  SOUND_ENGINE = 0,
  SOUND_CRASH,
  SOUND_START,
  SOUND_WIN,
  SOUND_LOSE
};

enum menu_fx { fx_action=0, fx_highlight };

#endif
