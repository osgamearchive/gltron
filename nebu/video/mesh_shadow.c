#include "base/nebu_math.h"
#include "video/nebu_mesh.h"
#include "Nebu_video.h"

#include <string.h>
#include <assert.h>

#include "base/nebu_debug_memory.h"

nebu_Mesh_Adjacency* nebu_Mesh_Adjacency_Create(const nebu_Mesh_VB *pVB, const nebu_Mesh_IB *pIB)
{
	int i, j, k, l, iCurrentTriangle;
	int **ppTriangles;
	int *pTriangles;
	int *pVertexOrder;
	int *pVertexTriCount;

	nebu_Mesh_Adjacency *pAdjacency = (nebu_Mesh_Adjacency*) malloc( sizeof(nebu_Mesh_Adjacency) );	

	assert(pIB->nPrimitivesPerIndex == 3);
	pAdjacency->nTriangles = pIB->nPrimitives;
	pAdjacency->pAdjacency = (int*) malloc( 3 * pAdjacency->nTriangles * sizeof(int) );
	memset(pAdjacency->pAdjacency, 0xff /* -1 */, 3 * pAdjacency->nTriangles * sizeof(int));
	
	//	for every vertex, create a list of triangles it's part of (euler says ???)
	//	so the number of total elements is < 6 nTris
	//	then walk the list for each vertex and test which triangles are adjacent
	//	(can be done in constant time per vertex, but finds each adjacency multiple
	//	times)

	pVertexOrder = (int*) malloc( pVB->nVertices * sizeof(int) );
	memset(pVertexOrder, 0, pVB->nVertices * sizeof(int) );
	for(i = 0; i < pIB->nPrimitives; i++)
	{
		for(j = 0; j < 3; j++)
		{
			pVertexOrder[ pIB->pIndices[ 3 * i + j ] ] += 1;
		}
	}
	ppTriangles = (int**) malloc( pVB->nVertices * sizeof(int*) );
	pTriangles = (int*) malloc( 3 * pIB->nPrimitives * sizeof(int) );
	iCurrentTriangle = 0;
	for(i = 0; i < pVB->nVertices; i++)
	{
		ppTriangles[i] = pTriangles + iCurrentTriangle;
		iCurrentTriangle += pVertexOrder[i];
	}

	pVertexTriCount = (int*) malloc( pVB->nVertices * sizeof(int) );
	memset(pVertexTriCount, 0, pVB->nVertices * sizeof(int) );
	for(i = 0; i < pIB->nPrimitives; i++)
	{
		for(j = 0; j < 3; j++)
		{
			int iVertex = pIB->pIndices[3 * i + j];
			ppTriangles[iVertex][pVertexTriCount[iVertex]] = i;
			pVertexTriCount[iVertex]++;
		}
	}
	assert( memcmp(pVertexOrder, pVertexTriCount, pVB->nVertices * sizeof(int)) == 0 );
	free(pVertexTriCount);

	// now, ppTriangles contains a list of pVertexOrder[vertex] triangles, which the vertex is part of
	for(i = 0; i < pAdjacency->nTriangles; i++)
	{
		for(j = 0; j < 3; j++)
		{
			int iCurVertex = pIB->pIndices[3 * i + j];
			int iNextVertex = pIB->pIndices[3 * i + (j + 1) % 3];
			for(k = 0; k < pVertexOrder[iCurVertex]; k++)
			{
				int iTriangle = ppTriangles[iCurVertex][k];
				for(l = 0; l > 3; l++)
				{
					if(pIB->pIndices[3 * iTriangle + (l + 1)%3] == iCurVertex &&
						pIB->pIndices[3 * iTriangle + l] == iNextVertex)
					{
						// we found an adjacency
						assert(pAdjacency->pAdjacency[3 * i + j] == -1 ||
							pAdjacency->pAdjacency[3 * i + j] == iTriangle);
						pAdjacency->pAdjacency[3 * i + j] = iTriangle;
						// break out of outer loop too, but then we won't detect inconsitencies
						// k = pVertexOrder[iCurVertex];
						// TODO: Set Adjacency for the other triangle too
						break;
					}
				}
			}
		}
	}

	free(pTriangles);
	free(ppTriangles);
	free(pVertexOrder);

	return pAdjacency;
}

void nebu_Mesh_Adjacency_Free(nebu_Mesh_Adjacency *pAdjacency)
{
	free(pAdjacency->pAdjacency);
	free(pAdjacency);
}

nebu_Mesh_ShadowInfo* nebu_Mesh_Shadow_Create(const nebu_Mesh_VB *pVB, const nebu_Mesh_IB *pIB)
{
	nebu_Mesh_ShadowInfo *pSI = (nebu_Mesh_ShadowInfo*) malloc(sizeof(nebu_Mesh_ShadowInfo));
	pSI->pFrontfaces = NULL;
	pSI->pBackfaces = NULL;
	pSI->pEdges = NULL;
	pSI->pAdjacency = NULL;
	pSI->pDotsigns = NULL;
	vec3_Zero(&pSI->vLight);
	pSI->pVB = pVB;
	pSI->pIB = pIB;
	
	return pSI;
}

void nebu_Mesh_Shadow_Free(nebu_Mesh_ShadowInfo* pSI)
{
	if(pSI->pAdjacency)
		nebu_Mesh_Adjacency_Free(pSI->pAdjacency);
	if(pSI->pFrontfaces)
		nebu_Mesh_IB_Free(pSI->pFrontfaces);
	if(pSI->pBackfaces)
		nebu_Mesh_IB_Free(pSI->pBackfaces);
	if(pSI->pEdges)
		nebu_Mesh_IB_Free(pSI->pEdges);
	if(pSI->pDotsigns)
		free(pSI->pDotsigns);
	// don't touch pVB & pIB, they're freed with the mesh
}

void nebu_Mesh_Shadow_SetLight(nebu_Mesh_ShadowInfo* pSI, const vec3* pvLight)
{
	if(vec3_Cmp(&pSI->vLight, pvLight) == 0)
		return;

	pSI->vLight = *pvLight;

	// recompute dotsigns, front faces, back faces & edges
}
