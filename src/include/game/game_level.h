#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "base/nebu_vector.h"

typedef struct {
	vec2 v;
	int dir;
} game_spawnpoint;

typedef struct {
	int scalable;
	int nBoundaries;
	segment2 *boundaries;
	int nSpawnPoints;
	game_spawnpoint *spawnPoints;
	box2 boundingBox;
} game_level;

void game_FreeLevel(game_level *l);
game_level* game_CreateLevel();
void game_ScaleLevel(game_level *l, float fSize);

#endif
