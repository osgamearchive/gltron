#include "video/nebu_mesh_3ds.h"

#include <lib3ds/file.h>                        
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include <stdlib.h>
#include <string.h>

static void countVerticesAndTriangles(nebu_Mesh_3ds_File *pFile, 
															 int *pnVertices, int *pnTriangles)
{
	Lib3dsMesh *p;
	for(p = pFile->meshes; p != NULL; p = p->next)
	{
		*pnVertices += p->points;
		*pnTriangles += p->faces;
	}
}

static void addToMesh(nebu_Mesh *pMesh, int *pCurVertex, int *pCurTri, Lib3dsMesh *pLib3dsMesh)
{
	int i, j;

	// TODO: add vertices & triangles of this node to mesh
	memcpy(pMesh->pVertices + 3 * *pCurVertex, pLib3dsMesh->pointL, pLib3dsMesh->points * 3 * sizeof(float));

	for(i = 0; i < pLib3dsMesh->faces; i++)
	{
		for(j = 0; j < 3; j++)
		{
			pMesh->pTriangles[3 * (i + *pCurTri) + j] = pLib3dsMesh->faceL[i].points[j] + *pCurVertex;
		}
	}
	*pCurVertex += pLib3dsMesh->points;
	*pCurTri += pLib3dsMesh->faces;
}


nebu_Mesh* nebu_Mesh_3ds_GetFromFile(nebu_Mesh_3ds_File* pFile)
{
	int nTriangles = 0;
	int nVertices = 0;

	countVerticesAndTriangles(pFile, &nVertices, &nTriangles);
	if(nVertices == 0 || nTriangles == 0) {
		return NULL;
	}

	nebu_Mesh* pMesh = nebu_Mesh_Create(NEBU_MESH_POSITION, nVertices, nTriangles);
	{
		int curVertex = 0;
		int curTri = 0;
		Lib3dsMesh *p = NULL;

		for(p = pFile->meshes; p != NULL; p = p->next) {
			addToMesh(pMesh, &curVertex, &curTri, p);
		}
	}
	return pMesh;
}

nebu_Mesh_3ds_File* nebu_Mesh_3ds_LoadFile(const char *filename)
{
	return lib3ds_file_load(filename);
}

nebu_Mesh* nebu_Mesh_3ds_Load(const char *filename) {
	nebu_Mesh* pMesh = NULL;
	nebu_Mesh_3ds_File *p3DSFile = nebu_Mesh_3ds_LoadFile(filename);
	if(!p3DSFile)
		return NULL;
	
	pMesh = nebu_Mesh_3ds_GetFromFile(p3DSFile);
	nebu_Mesh_3ds_FreeFile(p3DSFile);
	return pMesh;
}

void nebu_Mesh_3ds_FreeFile(nebu_Mesh_3ds_File* pFile) {
	lib3ds_file_free(pFile);
}
