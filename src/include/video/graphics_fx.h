#ifndef GRAPHICS_FX_H
#define GRAPHICS_FX_H

typedef struct PlayerVisual PlayerVisual;
typedef struct Camera Camera;
typedef struct Player Player;
struct Visual;

void drawImpact(PlayerVisual *pV);
void drawGlow(Camera *pCam, Player *pTarget, PlayerVisual *pV, 
	Visual *d, float dim);

#endif
