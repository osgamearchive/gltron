#include "video/video_level.h"

void video_free_level(video_level *l) {
}

video_level video_create_level(const char *name) {
	video_level *l;
	int i;

	l = malloc( sizeof(video_level) );

	scripting_GetGlobal("level", NULL);
	// get scalability flag
	scripting_GetValue("scalable");
	l->scalable = scripting_GetIntegerResult();

	// get floor & arena meshes
	scripting_GetValue("floor");
	mesh_load(& l->floor );
	scripting_PopTable();
	
	scripting_GetValue("arena");
	mesh_load(& l->arena );
	scripting_PopTable();

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
	

void mesh_load(mesh **ppMesh) {
	mesh *pMesh;
	int vertexSize;
	int i;
	void *pVertices;
	
	*ppmesh = malloc( sizeof(mesh) );
	pMesh = *ppmesh;
	
	scripting_GetValue("vertexformat");
	pMesh->vertexFormat = scripting_GetIntegerValue();
	vertexSize = 
		(pMesh->vertexFormat & MESH_POSITON) * 3 * sizeof(float) +
		(pMesh->vertexFormat & MESH_NORMAL) * 3 * sizeof(float) +
		(pMesh->vertexFormat & MESH_TEXCOORD0) * 2 * sizeof(float);

	scripting_GetValue("type");
	pMesh->primitiveType = scripting_GetIntegerValue();

	scripting_GetValue("vertices");
	pMesh->nVertices = scripting_GetArraySize();
	pMesh->pPackedVertices = malloc( pMesh->nVertices * vertexSize );
	pVertices = pMesh->pPackedVertices;
	
	for(i = 0; i < pMesh->nVertices; i++) {
		scripting_GetArrayIndex(i);
		if(pMesh->vertexFormat & MESH_POSITION) {
			scripting_GetValue("pos");
			scripting_GetValue("x");
			(float*) ((unsigned char*) pVertices + 0) =
				scripting_GetFloatValue();
			scripting_GetValue("y");
			(float*) ((unsigned char*) pVertices + 4) =
				scripting_GetFloatValue();
			scripting_GetValue("z");
			(float*) ((unsigned char*) pVertices + 8) =
				scripting_GetFloatValue();
			scripting_PopTable();
		}
		if(pMesh->vertexFormat & MESH_TEXCOORD0) {
			scripting_GetValue("uv");
			scripting_GetValue("u");
			scripting_GetValue("v");
			scripting_PopTable();
		}
		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // vertices
	
	scripting_GetValue("indices");
	pMesh->nFaces = scripting_GetArraySize();
	pMesh->pIndices = malloc( pMesh->nFaces * sizeof(int) );
	for(i = 0; i < pMesh->nFaces; i++) {
		scripting_GetArrayIndex(i);
		((int*)pMesh->pIndices)[i] = scripting_GetIntegerResult();
	}
	scripting_PopTable(); // indices
}
