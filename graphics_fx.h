#ifndef GRAPHICS_FX_H
#define GRAPHICS_FX_H

#include "gltron.h"

extern void drawImpact(Player *p);
extern void drawGlow(Camera *pCam, Player *pTarget, PlayerVisual *pV, 
										 Visual *d, float dim);

#endif
