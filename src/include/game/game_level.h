#ifndef GAME_LEVEL_H
#define VIDEO_LEVEL_H

#include "base/nebu_vector.h"

typedef struct {
	int scalable;
	int nBoundaries;
	segment2 *boundaries;
	int nSpawnPoints;
	vec2 *spawnPoints;
} game_level;

void game_free_level(game_level *l);
game_level* game_create_level(const char *name);

#endif
