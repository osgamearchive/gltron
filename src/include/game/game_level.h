#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "base/nebu_vector.h"

typedef struct {
	int scalable;
	int nBoundaries;
	segment2 *boundaries;
	int nSpawnPoints;
	vec2 *spawnPoints;
	box2 boundingBox;
} game_level;

void game_FreeLevel(game_level *l);
game_level* game_CreateLevel(const char *name);

#endif
