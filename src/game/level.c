#include "game/game_level.h"

void game_free_level(game_level *l) {
	free(l->boundaries);
	free(l->spawnPoints);
	free(l);
}

game_level* game_create_level(const char *name) {
	game_level* l;

	l = malloc( sizeof(game_level) );

	// fixme: load level 'name' from file
	scripting_GetGlobal("level", NULL);
	// get scalability flag
	scripting_GetValue("scalable");
	l->scalable = scripting_GetIntegerResult();
	// get number of spawnpoints
	scripting_GetValue("spawn");
	l->nSpawnPoints = scripting_GetArraySize();
	// copy spawnpoints into vec2's
	l->spawnPoints = malloc(l->nSpawnPoints * sizeof(vec2));
	for(i = 0; i < l->nSpawnPoints; i++) {
		scripting_GetArrayIndex(i);

		scripting_GetValue("x");
		l->spawnPoints[i].v[0] = scripting_GetFloatResult();
		scripting_GetValue("y");
		l->spawnPoints[i].v[1] = scripting_GetFloatResult();

		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // spawn
	
	// get number of boundary segments
	scripting_GetValue("boundary");
	l->nBoundaries = scripting_GetArraySize();
	// copy boundaries into segments
	l->boundaries = malloc(l->nBoundaries * sizeof(segment2));
	for(i = 0; i < l->nSpawnPonts; i++) {
		scripting_GetArrayIndex(i);
		
		scripting_GetArrayIndex(0);
		scripting_GetValue("x");
		l->boundaries[i].vStart.v[0] = scripting_GetFloatResult();
		scripting_GetValue("y");
		l->boundaries[i].vStart.v[1] = scripting_GetFloatResult();
		scripting_PopTable(); // index 0
		
		scripting_GetArrayIndex(1);
		scripting_GetValue("x");
		l->boundaries[i].vDirection.v[0] = 
			scripting_GetFloatResult() - l->boundaries[i].vStart.v[0];
		scripting_GetValue("y");
		l->boundaries[i].vDirection.v[1] =
			scripting_GetFloatResult() - l->boundaries[i].vStart.v[1];
		scripting_PopTable(); // index 1
	
		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // boundary

	scripting_PopTable(); // level
	return l;
}
