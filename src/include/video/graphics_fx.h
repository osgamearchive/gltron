#ifndef GRAPHICS_FX_H
#define GRAPHICS_FX_H

#include "video.h"
#include "game/game.h"

extern void drawImpact(PlayerVisual *pV);
extern void drawGlow(Camera *pCam, Player *pTarget, PlayerVisual *pV, 
										 Visual *d, float dim);

#endif
