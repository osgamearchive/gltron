#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

int initSound();
void shutdownSound();

int loadSound(char *name);
int playSound();
int stopSound();
void deleteSound();
void soundIdle();

void playGameFX(int fx);
void playMenuFX(int fx);

void playEngine();
void stopEngine();

enum game_fx { fx_engine=0, fx_start, fx_crash, fx_win, fx_lose };
enum menu_fx { fx_action=0, fx_highlight };



