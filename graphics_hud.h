#ifndef GRAPHICS_HUD_H
#define GRAPHICS_HUD_H

#include "gltron.h"

extern void drawAI(gDisplay *d);
extern void drawPause(gDisplay *d);

extern void drawScore(Player *p, gDisplay *d);
extern void drawFPS(gDisplay *d);

extern void drawConsoleLines(char *line, int call);
extern void drawConsole(gDisplay *d);

#endif
