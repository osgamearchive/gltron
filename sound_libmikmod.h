#ifndef SOUND_H
#define SOUND_H

#include <mikmod.h>

int initSound(void);
void shutdownSound(void);
void deleteSound(void);
void soundIdle(void);
int loadSound(char *name);
int playSound(void);
int stopSound(void);

/* compatibility functions */

void playGameFX(int fx);
void playMenuFX(int fx);

void playEngine();
void stopEngine();

enum game_fx { fx_engine=0, fx_start, fx_crash, fx_win, fx_lose };
enum menu_fx { fx_action=0, fx_highlight };

#endif

