#include <stdlib.h>

#include "video/nebu_mesh.h"
#include "video/video_level.h"

#include "nebu_Scripting.h"

nebu_Mesh* loadMesh(void);

void video_FreeLevel(video_level *l) {
}

void video_ScaleLevel(video_level *l, float fSize)
{
	nebu_Mesh_Scale(l->floor, fSize);
	nebu_Mesh_Scale(l->arena, fSize);
}

video_level* video_CreateLevel(void) {
	video_level *l;

	l = malloc( sizeof(video_level) );

	scripting_GetGlobal("level", NULL);
	// get scalability flag
	scripting_GetValue("scalable");
	scripting_GetIntegerResult(& l->scalable);

	scripting_GetValue("geometry");

	// get floor & arena meshes
	scripting_GetValue("floor");
	l->floor = loadMesh();
	scripting_PopTable();
	
	scripting_GetValue("arena");
	l->arena = loadMesh();
	scripting_PopTable();

	scripting_PopTable(); // geometry

	scripting_PopTable(); // level;

	return l;
}

enum {
	MESH_POSITION = 1,
	MESH_NORMAL = 2,
	MESH_TEXCOORD0 = 4
};

enum {
	V_POS = 0,
	V_NORMAL,
	V_TEXCOORD0
};
	

nebu_Mesh* loadMesh(void) {
	nebu_Mesh *pMesh;
	int i, j;
	
	pMesh = (nebu_Mesh*) malloc( sizeof(nebu_Mesh) );
	
	scripting_GetValue("vertexformat");
	scripting_GetIntegerResult(& pMesh->vertexformat);
	scripting_GetValue("vertices");
	scripting_GetArraySize(& pMesh->nVertices);

	if(pMesh->vertexformat & NEBU_MESH_POSITION)
		pMesh->pVertices = malloc( pMesh->nVertices * 3 * sizeof(float) );
	if(pMesh->vertexformat & NEBU_MESH_NORMAL)
		pMesh->pNormals = malloc( pMesh->nVertices * 3 * sizeof(float) );
	if(pMesh->vertexformat & NEBU_MESH_TEXCOORD)
		pMesh->pTexCoords = malloc( pMesh->nVertices * 2 * sizeof(float) );

	for(i = 0; i < pMesh->nVertices; i++) {
		scripting_GetArrayIndex(i + 1);
		if(pMesh->vertexformat & NEBU_MESH_POSITION) {
			scripting_GetValue("pos");
			scripting_GetValue("x");
			scripting_GetFloatResult( & pMesh->pVertices[3 * i + 0] );
			scripting_GetValue("y");
			scripting_GetFloatResult( & pMesh->pVertices[3 * i + 1] );
			scripting_GetValue("z");
			scripting_GetFloatResult( & pMesh->pVertices[3 * i + 2] );
			scripting_PopTable(); // pos
		}
		if(pMesh->vertexformat & NEBU_MESH_NORMAL) {
			scripting_GetValue("normal");
			scripting_GetValue("x");
			scripting_GetFloatResult( & pMesh->pNormals[3 * i + 0] );
			scripting_GetValue("y");
			scripting_GetFloatResult( & pMesh->pNormals[3 * i + 1] );
			scripting_GetValue("z");
			scripting_GetFloatResult( & pMesh->pNormals[3 * i + 2] );
			scripting_PopTable(); // pos
		}
		if(pMesh->vertexformat & NEBU_MESH_TEXCOORD) {
			scripting_GetValue("uv");
			scripting_GetValue("u");
			scripting_GetFloatResult( & pMesh->pTexCoords[2 * i + 0] );
			scripting_GetValue("v");
			scripting_GetFloatResult( & pMesh->pTexCoords[2 * i + 1] );
			scripting_PopTable(); // uv
		}
		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // vertices
	
	scripting_GetValue("indices");
	scripting_GetArraySize(& pMesh->nTriangles);
	pMesh->pTriangles = malloc( pMesh->nTriangles * 3 * sizeof(int) );
	for(i = 0; i < pMesh->nTriangles; i++) {
		scripting_GetArrayIndex(i + 1);
		for(j = 0; j < 3; j++)
		{
			scripting_GetArrayIndex(j + 1);
			scripting_GetIntegerResult( & pMesh->pTriangles[3 * i + j] );
		}
		scripting_PopTable(); // index i;
	}
	scripting_PopTable(); // indices

	return pMesh;
}
