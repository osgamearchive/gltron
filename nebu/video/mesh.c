#include "base/nebu_math.h"
#include "video/nebu_mesh.h"
#include "Nebu_video.h"

#include <string.h>

#include "base/nebu_debug_memory.h"

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

void nebu_Mesh_DrawGeometry(nebu_Mesh *pMesh)
{
	int i;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pMesh->vb.pVertices);

	for(i = 0; i < NEBU_MESH_TEXCOORD_MAXCOUNT; i++)
	{
		if(pMesh->vb.vertexformat & (NEBU_MESH_TEXCOORD0 << i)) {
			glClientActiveTexture(GL_TEXTURE0_ARB + i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if(pMesh->vb.pTexCoords[i])
				glTexCoordPointer(2, GL_FLOAT, 0, pMesh->vb.pTexCoords[i]);
			else
				glTexCoordPointer(2, GL_FLOAT, 0, pMesh->vb.pTexCoords[0]);
		}
	}

	if(pMesh->vb.vertexformat & NEBU_MESH_NORMAL) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, pMesh->vb.pNormals);
	}
	glDrawElements(GL_TRIANGLES, 3 * pMesh->nTriangles, GL_UNSIGNED_INT, pMesh->pTriangles);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	for(i = 0; i < NEBU_MESH_TEXCOORD_MAXCOUNT; i++)
	{
		if(pMesh->vb.vertexformat & (NEBU_MESH_TEXCOORD0 << i)) {
			glClientActiveTexture(GL_TEXTURE0_ARB + i);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	glClientActiveTexture(GL_TEXTURE0_ARB);
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
		v1[i] = pMesh->vb.pVertices[3 * b + i] - pMesh->vb.pVertices[3 * a + i];
		v2[i] = pMesh->vb.pVertices[3 * c + i] - pMesh->vb.pVertices[3 * a + i];
	}
	normal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	normal[1] = v1[2] * v2[0] - v1[0] * v2[2];
	normal[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void nebu_Mesh_ComputeNormals(nebu_Mesh *pMesh)
{
	int i, j;

	if(!pMesh->vb.pNormals)
		pMesh->vb.pNormals = (float*) malloc( 3 * sizeof(float) * pMesh->vb.nVertices );
	pMesh->vb.vertexformat |= NEBU_MESH_NORMAL;

	memset(pMesh->vb.pNormals, 0, 3 * sizeof(float) * pMesh->vb.nVertices);
	for(i = 0; i < pMesh->nTriangles; i++)
	{
		float normal[3];
		nebu_Mesh_ComputeTriangleNormal(pMesh, i, normal);

		for(j = 0; j < 3; j++)
		{
			int vertex = pMesh->pTriangles[3 * i + j];
			pMesh->vb.pNormals[3 * vertex + 0] += normal[0];
			pMesh->vb.pNormals[3 * vertex + 1] += normal[1];
			pMesh->vb.pNormals[3 * vertex + 2] += normal[2];
		}
	}
	for(i = 0; i < pMesh->vb.nVertices; i++)
	{
		normalize(pMesh->vb.pNormals + 3 * i);
	}
}

void nebu_Mesh_Scale(nebu_Mesh *pMesh, float fScale)
{
	int i, j;
	for(i = 0; i < pMesh->vb.nVertices; i++)
	{
		for(j = 0; j < 3; j++)
		{
			pMesh->vb.pVertices[3 * i + j] *= fScale;
		}
	}
}

void nebu_Mesh_Free(nebu_Mesh *pMesh)
{
	int i;

	free(pMesh->pTriangles);
	if(pMesh->vb.vertexformat & NEBU_MESH_POSITION)
		free(pMesh->vb.pVertices);
	if(pMesh->vb.vertexformat & NEBU_MESH_NORMAL)
		free(pMesh->vb.pNormals);

	for(i = 0; i < NEBU_MESH_TEXCOORD_MAXCOUNT; i++)
	{
		if(pMesh->vb.vertexformat & (NEBU_MESH_TEXCOORD0 << i) &&
			pMesh->vb.pTexCoords[i])
			free(pMesh->vb.pTexCoords[i]);
	}
	free(pMesh);
}
nebu_Mesh* nebu_Mesh_Create(int flags, int nVertices, int nTriangles)
{
	// TODO: add error checking
	int i;

	nebu_Mesh *pMesh = (nebu_Mesh*) malloc(sizeof(nebu_Mesh));
	pMesh->vb.nVertices = nVertices;
	pMesh->nTriangles = nTriangles;

	if(flags & NEBU_MESH_POSITION)
		pMesh->vb.pVertices = (float*) malloc(3 * sizeof(float) * nVertices);
	else
		pMesh->vb.pVertices = NULL;

	if(flags & NEBU_MESH_NORMAL)
		pMesh->vb.pNormals = (float*) malloc(3 * sizeof(float) * nVertices);
	else
		pMesh->vb.pNormals = NULL;

	for(i = 0; i < NEBU_MESH_TEXCOORD_MAXCOUNT; i++)
	{
		if(flags & (NEBU_MESH_TEXCOORD0 << i))
			pMesh->vb.pTexCoords[i] = (float*) malloc(2 * sizeof(float) * nVertices);
		else
			pMesh->vb.pTexCoords[i] = NULL;
	}

	pMesh->pTriangles = (int*) malloc(3 * sizeof(int) * nTriangles);
	return pMesh;
}

void nebu_Mesh_ComputeBBox(nebu_Mesh *pMesh, box3* box)
{
	box3_Compute(box, (vec3*)pMesh->vb.pVertices, pMesh->vb.nVertices);
}
