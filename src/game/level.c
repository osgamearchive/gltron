#include "game/game_level.h"
#include "Nebu_scripting.h"
#include <stdlib.h>

void game_FreeLevel(game_level *l) {
	free(l->boundaries);
	free(l->spawnPoints);
	free(l);
}

game_level* game_CreateLevel(const char *name) {
	int i;
	game_level* l;

	l = malloc( sizeof(game_level) );

	// fixme: load level 'name' from file
	scripting_GetGlobal("level", NULL);
	// get scalability flag
	scripting_GetValue("scalable");
	scripting_GetIntegerResult(& l->scalable);
	// get number of spawnpoints
	scripting_GetValue("spawn");
	scripting_GetArraySize(& l->nSpawnPoints);
	// copy spawnpoints into vec2's
	l->spawnPoints = malloc(l->nSpawnPoints * sizeof(vec2));
	for(i = 0; i < l->nSpawnPoints; i++) {
		scripting_GetArrayIndex(i);

		scripting_GetValue("x");
		scripting_GetFloatResult(& l->spawnPoints[i].v[0]);
		scripting_GetValue("y");
		scripting_GetFloatResult(& l->spawnPoints[i].v[1]);

		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // spawn
	
	// get number of boundary segments
	scripting_GetValue("boundary");
	l->nBoundaries = scripting_GetArraySize();
	// copy boundaries into segments
	l->boundaries = malloc(l->nBoundaries * sizeof(segment2));
	for(i = 0; i < l->nSpawnPoints; i++) {
		scripting_GetArrayIndex(i);
		
		scripting_GetArrayIndex(0);
		scripting_GetValue("x");
		scripting_GetFloatResult(& l->boundaries[i].vStart.v[0]);
		scripting_GetValue("y");
		scripting_GetFloatResult(& l->boundaries[i].vStart.v[1]);
		scripting_PopTable(); // index 0
		
		scripting_GetArrayIndex(1);
		{
			vec2 v;
			scripting_GetValue("x");
			scripting_GetFloatResult(& v.v[0]);
			scripting_GetValue("y");
			scripting_GetFloatResult(& v.v[1]);
			vec2_Sub(& l->boundaries[i].vDirection, &v, &l->boundaries[i].vStart);
		}
		scripting_PopTable(); // index 1
	
		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // boundary

	scripting_PopTable(); // level
	return l;
}
