#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "video/nebu_mesh.h"

static void normalize(float *v)
{
	float fSqrLength = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if(fSqrLength != 0)
	{
		float fLength = (float) sqrt(fSqrLength); 
		v[0] /= fLength;
		v[1] /= fLength;
		v[2] /= fLength;
	}
}

void nebu_Mesh_ComputeTriangleNormal(nebu_Mesh *pMesh, int triangle, float* normal)
{
	float v1[3], v2[3];
	int a, b, c, i;

	a = pMesh->pTriangles[3 * triangle + 0];
	b = pMesh->pTriangles[3 * triangle + 1];
	c = pMesh->pTriangles[3 * triangle + 2];
	for(i = 0; i < 3; i++)
	{
		v1[i] = pMesh->pVertices[3 * b + i] - pMesh->pVertices[3 * a + i];
		v2[i] = pMesh->pVertices[3 * c + i] - pMesh->pVertices[3 * a + i];
	}
	normal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	normal[1] = v1[2] * v2[0] - v1[0] * v2[2];
	normal[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void nebu_Mesh_ComputeNormals(nebu_Mesh *pMesh)
{
	int i, j;

	if(!pMesh->pNormals)
		pMesh->pNormals = (float*) malloc( 3 * sizeof(float) * pMesh->nVertices );

	memset(pMesh->pNormals, 0, 3 * sizeof(float) * pMesh->nVertices);
	for(i = 0; i < pMesh->nTriangles; i++)
	{
		float normal[3];
		nebu_Mesh_ComputeTriangleNormal(pMesh, i, normal);

		for(j = 0; j < 3; j++)
		{
			int vertex = pMesh->pTriangles[3 * i + j];
			pMesh->pNormals[3 * vertex + 0] += normal[0];
			pMesh->pNormals[3 * vertex + 1] += normal[1];
			pMesh->pNormals[3 * vertex + 2] += normal[2];
		}
	}
	for(i = 0; i < pMesh->nVertices; i++)
	{
		normalize(pMesh->pNormals + 3 * i);
	}
}

void nebu_Mesh_Scale(nebu_Mesh *pMesh, float fScale)
{
	int i, j;
	for(i = 0; i < pMesh->nVertices; i++)
	{
		for(j = 0; j < 3; j++)
		{
			pMesh->pVertices[3 * i + j] *= fScale;
		}
	}
}

nebu_Mesh* nebu_Mesh_Create(int flags, int nVertices, int nTriangles)
{
	// TODO: add error checking

	nebu_Mesh *pMesh = (nebu_Mesh*) malloc(sizeof(nebu_Mesh));
	pMesh->nVertices = nVertices;
	pMesh->nTriangles = nTriangles;

	if(flags & NEBU_MESH_POSITION)
		pMesh->pVertices = (float*) malloc(3 * sizeof(float) * nVertices);
	else
		pMesh->pVertices = NULL;

	if(flags & NEBU_MESH_NORMAL)
		pMesh->pNormals = (float*) malloc(3 * sizeof(float) * nVertices);
	else
		pMesh->pNormals = NULL;

	pMesh->pTriangles = (int*) malloc(3 * sizeof(int) * nTriangles);
	return pMesh;
}
