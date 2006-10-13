#include "game/game_level.h"
#include "game/resource.h"
#include "video/model.h"
#include "video/video.h"
#include "filesystem/path.h"
#include "Nebu_scripting.h"

#include "base/nebu_debug_memory.h"

#include "base/nebu_assert.h"
#include <math.h>

void game_FreeLevel(game_level *l) {
	free(l->boundaries);
	free(l->spawnPoints);
	free(l);
}

/*! Scales the level by the factor fScale

	- The boundary segments are scaled
	- The boundingbox is scaled
	- If the spawn coordinates are not relative to the bounding box, they
	  are scaled along accordingly
*/
void game_ScaleLevel(game_level *l, float fScale)
{
	int i;
	for(i = 0; i < l->nBoundaries; i++)
	{
		segment2_Scale(&l->boundaries[i], fScale);
	}
	if(!l->spawnIsRelative)
	{
		for(i = 0; i < l->nSpawnPoints; i++)
		{
			vec2_Scale(& l->spawnPoints[i].v, & l->spawnPoints[i].v, fScale);
		}
	}

	box2_Scale(&l->boundingBox, fScale);
}

void computeBoundingBox(game_level *l)
{
	int i;

	box2_Init(& l->boundingBox);
	for(i = 0; i < l->nBoundaries; i++)
	{
		vec2 vEnd;
		vec2_Add(&vEnd, & l->boundaries[i].vStart, & l->boundaries[i].vDirection);
		box2_Extend(& l->boundingBox, & l->boundaries[i].vStart);
		box2_Extend(& l->boundingBox, & vEnd);
	}
}

void computeBoundaries(game_level *l)
{
	int i, j;
	gltron_Mesh *pFloor = resource_Get(gpTokenCurrentFloor, eRT_GLtronTriMesh);
	int iCurIndex;

	nebu_assert(pFloor);
	nebu_assert(pFloor->pSI);

	if(!pFloor->pSI->pAdjacency)
	{
		pFloor->pSI->pAdjacency = nebu_Mesh_Adjacency_Create(pFloor->pSI->pVB, pFloor->pSI->pIB);
	}

	l->nBoundaries = 0;
	for(i = 0; i < pFloor->pSI->pAdjacency->nTriangles; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(pFloor->pSI->pAdjacency->pAdjacency[3 * i + j] == -1)
			{
				l->nBoundaries += 1;
			}
		}
	}
	l->boundaries = malloc(l->nBoundaries * sizeof(segment2));

	iCurIndex = 0;
	for(i = 0; i < pFloor->pSI->pAdjacency->nTriangles; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(pFloor->pSI->pAdjacency->pAdjacency[3 * i + j] == -1)
			{
				vec3 *pV1, *pV2;
				pV1 = (vec3*)(pFloor->pSI->pVB->pVertices +
					3 * pFloor->pSI->pIB->pIndices[3 * i + j]);
				pV2 = (vec3*)(pFloor->pSI->pVB->pVertices +
					3 * pFloor->pSI->pIB->pIndices[3 * i + (j + 1) % 3]);
					
				nebu_assert(iCurIndex < l->nBoundaries);
				{
					float fEpsilon = 0.001f;
					nebu_assert(fabs(pV1->v[2]) < fEpsilon);
					nebu_assert(fabs(pV2->v[2]) < fEpsilon);
				}
				l->boundaries[iCurIndex].vStart = *(vec2*)pV1;
				vec2_Sub(&l->boundaries[iCurIndex].vDirection, (vec2*)pV2, (vec2*)pV1);

				iCurIndex++;
			}
		}
	}
	nebu_assert(iCurIndex == l->nBoundaries);
}	

void game_UnloadLevel(void)
{
	// free all loaded mesh resources & textures
	video_UnloadLevel();

	// delete the current (global) lua table
	// delete global 'level' table (garbage collected)
	scripting_Run("level = nil");
}

void game_LoadLevel(void)
{
	// Load the selected level into the (global) lua table
	char *path; 
	char *pFilename;

	int iPos = scripting_StackGuardStart();

	// make sure there's no level already loaded
	scripting_GetGlobal("level", NULL);
	nebu_assert(scripting_IsNil());
	scripting_Pop();

	scripting_GetGlobal("settings", "current_level", NULL);
	scripting_GetStringResult(&pFilename);
	fprintf(stderr, "[status] loading level '%s'\n", pFilename);
	path = getPath(PATH_LEVEL, pFilename);
	free(pFilename);

	if(path) {
		scripting_RunFile(path);
		free(path);
	}
	else
	{
		printf("[fatal] can't get valid path for level\n");
		nebu_assert(0); exit(1); // fatal
	}

	scripting_GetGlobal("level", NULL);
	nebu_assert(!scripting_IsNil());
	scripting_Pop(); // level

	scripting_StackGuardEnd(iPos);
}

game_level* game_CreateLevel(void)
{
	int i;
	game_level* l;
	int iHasBoundary;

	l = malloc( sizeof(game_level) );
	scripting_GetGlobal("level", NULL);
	nebu_assert(!scripting_IsNil());
	// get scalability flag
	scripting_GetValue("scalable");
	scripting_GetIntegerResult(& l->scalable);
	// get default scale
	scripting_GetValue("default_scale");
	if(!scripting_IsNil())
	{
		scripting_GetFloatResult(& l->default_scale);
	}
	else
	{
		l->default_scale = 1;
		scripting_Pop();
	}
	// are spawn points relative?
	scripting_GetValue("spawn_is_relative");
	if(scripting_IsNil())
	{
		scripting_Pop();
		l->spawnIsRelative = 1;
	}
	else
	{
		scripting_GetIntegerResult(& l->spawnIsRelative);
	}
	// get number of spawnpoints
	scripting_GetValue("spawn");
	scripting_GetArraySize(& l->nSpawnPoints);
	// copy spawnpoints into vec2's
	l->spawnPoints = malloc(l->nSpawnPoints * sizeof(game_spawnpoint));

	// Spawn points are relative to the bounding box of the fllor
	for(i = 0; i < l->nSpawnPoints; i++) {
		scripting_GetArrayIndex(i + 1);

		scripting_GetValue("x");
		scripting_GetFloatResult(& l->spawnPoints[i].v.v[0]);
		scripting_GetValue("y");
		scripting_GetFloatResult(& l->spawnPoints[i].v.v[1]);
		scripting_GetValue("dir");
		scripting_GetIntegerResult(& l->spawnPoints[i].dir);

		scripting_Pop(); // index i
	}
	scripting_Pop(); // spawn
	
	// TODO: in testing

	// two possibilities:
	// 1) The level contains boundary, vertices & indices fields,
	//    so we can load everything from there
	// 2) There's a 'model' field, and the boundaries	are the triangle edges
	//    without an adjacency

	scripting_GetValue("boundary");
	iHasBoundary = scripting_IsNil() ? 0 : 1;
	scripting_Pop();

	if(iHasBoundary)
	{
		// store boundary from lua
		scripting_GetValue("boundary");

		// get number of boundary segments
		scripting_GetArraySize(& l->nBoundaries);
		// copy boundaries into segments
		l->boundaries = malloc(l->nBoundaries * sizeof(segment2));
		for(i = 0; i < l->nBoundaries; i++)
		{
			scripting_GetArrayIndex(i + 1);
			
			scripting_GetArrayIndex(1);
			scripting_GetValue("x");
			scripting_GetFloatResult(& l->boundaries[i].vStart.v[0]);
			scripting_GetValue("y");
			scripting_GetFloatResult(& l->boundaries[i].vStart.v[1]);
			scripting_Pop(); // index 0
			
			scripting_GetArrayIndex(2);
			{
				vec2 v;
				scripting_GetValue("x");
				scripting_GetFloatResult(& v.v[0]);
				scripting_GetValue("y");
				scripting_GetFloatResult(& v.v[1]);
				vec2_Sub(& l->boundaries[i].vDirection, &v, &l->boundaries[i].vStart);
			}
			scripting_Pop(); // index 1
		
			scripting_Pop(); // index i
		}
		scripting_Pop(); // boundary
	}		
	else
	{
		if(!gpTokenCurrentFloor)
		{
			// needs either floor mesh or boundary description
			fprintf(stderr, "fatal: arena incomplete - exiting...\n");
			nebu_assert(0); exit(1); // OK (somewhat): the level is unplayable. could be handled better
		}
		// compute boundaries from level model
		computeBoundaries(l);
	}

	scripting_Pop(); // level

	computeBoundingBox(l);

	return l;
}
