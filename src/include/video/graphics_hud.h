#ifndef GRAPHICS_HUD_H
#define GRAPHICS_HUD_H

#include "video.h"
#include "game/game.h"

extern void drawHUD(Player *p, PlayerVisual *pV);

extern void drawFPS(Visual *d);
extern void drawPause(Visual *d);

extern void drawConsoleLines(char *line, int call);
extern void drawConsole(Visual *d);

#endif
