// include a few datastructures & constants

#include "video/model.h"
#include "video/video.h"

#include "base/nebu_math.h"
#include "video/nebu_renderer_gl.h"
#include <stdio.h>
#include <zlib.h>
#include <string.h>
#include <assert.h>

#include "base/nebu_debug_memory.h"

typedef struct face {
  int vertex[3];
  int normal[3];
  int material;
} face;

typedef struct quadFace {
 int vertex[4];
 int normal[4];
 int material;
} quadFace;

extern void readMaterialLibrary(char *buf, gltron_Mesh *pMesh);
extern void setMaterial(char *buf, gltron_Mesh *pMesh, int *iGroup);

void readVector(char *buf, vec3 *pVertex ) {
  if( sscanf(buf, " %f %f %f ", 
	     pVertex->v, pVertex->v + 1, pVertex->v + 2) != 3) {
    fprintf(stderr, "*** failed reading Vector from %s\n", buf);
  }
}

void readTriFace(char *buf, face *pFace, int *iFace, int iGroup) {
	int dummy;
	if(
		// assumes model in "f %d//%d %d//%d %d//%d\n" format
		sscanf(buf, "f %d//%d %d//%d %d//%d ",
			pFace[ *iFace ].vertex + 0, pFace[ *iFace ].normal + 0,
			pFace[ *iFace ].vertex + 1, pFace[ *iFace ].normal + 1,
			pFace[ *iFace ].vertex + 2, pFace[ *iFace ].normal + 2) == 6)
	{
		pFace[ *iFace ].material = iGroup;
		(*iFace)++;
	}
	// FIXME: load texture coordinates instead of throwing them away
	else if(
		sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d ",
			pFace[ *iFace ].vertex + 0, &dummy, pFace[ *iFace ].normal + 0,
			pFace[ *iFace ].vertex + 1, &dummy, pFace[ *iFace ].normal + 1,
			pFace[ *iFace ].vertex + 2, &dummy, pFace[ *iFace ].normal + 2) == 9)
	{
		pFace[ *iFace ].material = iGroup;
		(*iFace)++;
	}
	else
	{
		fprintf(stderr, "*** failed parsing face %s\n", buf);
	}
}

void readQuadFace(char *buf, quadFace *pFace, int *iFace, int iGroup) {
// assumes model in "f %d//%d %d//%d %d//%d %d//%d\n" format
  if(
     sscanf(buf, "f %d//%d %d//%d %d//%d %d//%d",
            pFace[ *iFace ].vertex + 0, pFace[ *iFace ].normal + 0,
            pFace[ *iFace ].vertex + 1, pFace[ *iFace ].normal + 1,
            pFace[ *iFace ].vertex + 2, pFace[ *iFace ].normal + 2,
            pFace[ *iFace ].vertex + 3, pFace[ *iFace ].normal + 3) == 8) {
    pFace[ *iFace ].material = iGroup;
    (*iFace)++;
  } else {
    fprintf(stderr, "*** failed parsing face %s\n", buf);
  }
}

void gltron_Mesh_Free(gltron_Mesh* pMesh)
{
	int i;

	for(i = 0; i < pMesh->nMaterials; i++)
	{
		free(pMesh->pMaterials[i].name);
		free(pMesh->ppIndices[i]);
	}
	free(pMesh->pMaterials);
	free(pMesh->ppIndices);
	free(pMesh->pnFaces);
	nebu_Mesh_VB_Free(pMesh->pVB);
	if(pMesh->pSI)
	{
		nebu_Mesh_Shadow_Free(pMesh->pSI);
	}
	free(pMesh);
}

	// what we want is to
	// 1) compute the order of each vertex
	// 2) allocate the max. amount of necessary memory (i.e. if all vertices need doubling
	// 3) create the vertices as needed, keeping track of how often vertices are doubled
	// 4) finally compact the vertex array again, and compute the 'new' triangle indices

	// this also creates an index-remapping
void createVertices(
	nebu_Mesh_VB **ppVB,
	face *pFaces, int nFaces,
	vec3 *pVertices, int nVertices,
	vec3 *pNormals, int nNormals)
{
	int i, j, k;

	// 1) compute the order of each vertex
	// 2) allocate the max. amount of necessary memory (i.e. if all vertices need doubling)
	// 3) create the vertices as needed, keeping track of how often vertices are doubled
	// 4) finally compact the vertex array again, and compute the 'new' triangle indices

	// this also creates an index-remapping
	int *piVertexOrder; // stores the order for each vertex
	int *piUsedVertexCount; // stores how many times each vertex is 'instanced' (used)
	unsigned char *pVertexData; // actual (uncompacted) vertex buffer	
	int *piVertexPosition;
	int iCurIndex;
	unsigned char *pTmpVertex;
	int *pIndices;

	int iVertexSize = 6 * sizeof(float);
	
	pIndices = malloc( 3 * nFaces * sizeof(int) );
	piVertexOrder = (int*) malloc( nVertices * sizeof(int) );
	memset(piVertexOrder, 0, nVertices * sizeof(int));
	
	// 1) compute the order of each vertex
	for(i = 0; i < nFaces; i++)
	{
		for(j = 0; j < 3; j++)
		{
			// TODO: subtract 1 from the index in the reading routine
			piVertexOrder[ pFaces[i].vertex[j] - 1 ] += 1;
		}
	}
	
	// 2) allocate the max. amount of necessary memory (i.e. if all vertices need doubling)
	pVertexData = malloc( 3 * nFaces * iVertexSize );

	piVertexPosition = (int*) malloc( nVertices * sizeof(int) );
	iCurIndex = 0;
	for(i = 0; i < nVertices; i++)
	{
		piVertexPosition[i] = iCurIndex;
		iCurIndex += piVertexOrder[i];
	}

	piUsedVertexCount = (int*) malloc( nVertices * sizeof(int) );
	memset(piUsedVertexCount, 0, nVertices * sizeof(int));
	
	// 3) create the vertices as needed, keeping track of how often vertices are doubled
	// store the computed indices in the temporary index array

	pTmpVertex = (unsigned char*) malloc( iVertexSize );

	for(i = 0; i < nFaces; i++)
	{
		for(j = 0; j < 3; j++)
		{
			// TODO: subtract 1 from the index in the reading routine
			int iVertex = pFaces[i].vertex[j] - 1;
			int iNormal = pFaces[i].normal[j] - 1;

			int offset = 0;
			int index = -1;
			
			// build vertex
			memcpy(pTmpVertex, pVertices + iVertex, sizeof(vec3));
			offset += sizeof(vec3);
			memcpy(pTmpVertex + offset, pNormals + iNormal, sizeof(vec3));
			offset += sizeof(vec3);

			assert(piVertexOrder[iVertex] > 0);

			for(k = 0; i < piUsedVertexCount[iVertex]; i++)
			{
				if(memcmp(pTmpVertex, pVertexData + (piVertexPosition[iVertex] + k) * iVertexSize, iVertexSize) == 0)
				{
					// vertex schon vorhanden
					index = piVertexPosition[iVertex] + k;
				}
			}
			if(index == -1)
			{
				// create new vertex
				assert(piUsedVertexCount[iVertex] < piVertexOrder[iVertex]);
				index = piVertexPosition[iVertex] + piUsedVertexCount[iVertex];
				memcpy(pVertexData + index * iVertexSize,
					pTmpVertex, iVertexSize);
				piUsedVertexCount[iVertex] += 1;
			}
			// store the 'new' triangle index
			pIndices[3 * i + j] = index;
		}
	}

	free(pTmpVertex);

	// 4) finally compact the vertex array again
	{
		int iVertexCount = 0;
		int iUsedVertex = 0;
		for(i = 0; i < nVertices; i++)
		{
			iVertexCount += piUsedVertexCount[i];
		}
		*ppVB = nebu_Mesh_VB_Create(NEBU_MESH_POSITION | NEBU_MESH_NORMAL, iVertexCount);

		iCurIndex = 0;
		for(i = 0; i < nVertices; i++)
		{
			for(j = 0; j < piUsedVertexCount[i]; j++)
			{
				assert(iUsedVertex < iVertexCount);
				memcpy((*ppVB)->pVertices + 3 * iUsedVertex,
					pVertexData + (iCurIndex + j) * iVertexSize, 3 * sizeof(float));
				memcpy((*ppVB)->pNormals + 3 * iUsedVertex,
					pVertexData + (iCurIndex + j) * iVertexSize + 3 * sizeof(float), 3 * sizeof(float));
				iUsedVertex++;
			}
			iCurIndex += piVertexOrder[i];
		}
	}
	free(piUsedVertexCount);
	free(piVertexPosition);
	free(pVertexData);
	free(piVertexOrder);
	
	// store new indices in face list, and invalidate normals
	for(i = 0; i < nFaces; i++)
	{
		for(j = 0; j < 3; j++)
		{
			pFaces[i].vertex[j] = pIndices[3 * i + j];
			pFaces[j].normal[j] = -1;
		}
	}
	free(pIndices);
}

gltron_Mesh* gltron_Mesh_LoadFromFile(const char *filename, gltron_MeshType iType)
{
	// allocate some buffers
	// vertices, normals

	vec3 *pVertices = malloc( sizeof(vec3) * MAX_VERTICES );
	vec3 *pNormals = malloc( sizeof(vec3) * MAX_NORMALS );

	quadFace *pqFaces = NULL;
	face *pFaces = NULL;
	int iFaceSize = 0;

	gltron_Mesh *pMesh;

	int iGroup = 0;
	int iVertex = 0, iNormal = 0, iFace = 0;

	gzFile f;
	char buf[BUF_SIZE];

	int i, j, k;

	pMesh = malloc( sizeof(gltron_Mesh) );
	memset(pMesh, 0, sizeof(gltron_Mesh));

	switch(iType)
	{
	case TRI_MESH:
		pFaces = malloc( sizeof(face) * MAX_FACES );
		iFaceSize = 3;
		break;
	case QUAD_MESH:
		pqFaces = malloc( sizeof(quadFace) * MAX_FACES );
		iFaceSize = 4;
		break;
	default:
		fprintf(stderr, "[fatal]: illegal mesh type\n");
		exit(1);
	}

	if((f = gzopen(filename, "r")) == 0)
	{
		fprintf(stderr, "*** could not open file '%s'\n", filename);
		return NULL;
	}

	while( gzgets(f, buf, sizeof(buf)) )
	{
		switch(buf[0])
		{
		case 'm': // material library
			readMaterialLibrary(buf, pMesh);
			break;
		case 'u': // material  name
			setMaterial(buf, pMesh, &iGroup);
			break;
		case 'v': // vertex, normal, texture coordinate
			switch(buf[1])
			{
			case ' ':
				readVector(buf + 1, pVertices + iVertex);
				iVertex++;
				break;
			case 'n': // normal
				readVector(buf + 2, pNormals + iNormal);
				iNormal++;
				break;
			case 't': // texture
				break; // ignore textures;
			}
			break;
		case 'f': // face (can produce multiple faces)
			switch(iType)
			{
			case TRI_MESH:
				readTriFace(buf, pFaces, &iFace, iGroup);
				break;
			case QUAD_MESH:
				readQuadFace(buf, pqFaces, &iFace, iGroup);
				break;
			}
			break;
		}
	}
	  
	gzclose(f);

	// printf("vertices: %d, normals: %d, faces: %d\n", iVertex, iNormal, iFace);

	// count each material
	pMesh->pnFaces = malloc( sizeof(int) * pMesh->nMaterials );
	for(i = 0; i < pMesh->nMaterials; i++) {
		pMesh->pnFaces[ i ] = 0;
	}

	switch(iType) {
	case TRI_MESH:
		for(i = 0; i < iFace; i++)
		{
			pMesh->pnFaces[ pFaces[i].material ] += 1;
		}
		break;
	case QUAD_MESH:
		for(i = 0; i < iFace; i++)
		{
			pMesh->pnFaces[ pqFaces[i].material ] += 1;
		}
		break;
	}

	// ok, now we have the mesh data in memory
	// let's build a vertex & index buffer we can use for shadow volume calculations
	if(iType == TRI_MESH && 
		(strstr(filename, "ultra") ||
		0) // strstr(filename, "high"))
		)
	{
		nebu_Mesh_VB *pVB;
		nebu_Mesh_IB *pIB;

		pVB = nebu_Mesh_VB_Create( NEBU_MESH_POSITION, iVertex );
		pIB = nebu_Mesh_IB_Create( iFace, 3 );
	
		for(i = 0; i < iFace; i++)
		{
			for(j = 0; j < 3; j++)
			{
				pIB->pIndices[3 * i + j] = pFaces[i].vertex[j] - 1;
				assert(pIB->pIndices[3 * i + j] != -1);
			}
		}
		for(i = 0; i < iVertex; i++)
		{
			vec3_Copy( (vec3*)(pVB->pVertices + 3 * i), pVertices + i);
		}
		pMesh->pSI = nebu_Mesh_Shadow_Create(pVB, pIB);
	}
	// combine vectors & normals for each vertex, doubling verticees where necessary

	// the old implementation (see QUAD_MESH) is extremly inefficient and eats too much memory!
	if(iType == TRI_MESH)
	{
		createVertices(
			&pMesh->pVB,
			pFaces, iFace,
			pVertices, iVertex,
			pNormals, iNormal);

		// build indices (per Material)
		{
			int *face;
			face = malloc( sizeof(int) * pMesh->nMaterials );
			pMesh->ppIndices = malloc( sizeof(GLshort*) * pMesh->nMaterials );
			for(i = 0; i < pMesh->nMaterials; i++) {
				pMesh->ppIndices[i] = 
				malloc( sizeof(GLshort) * 3 * pMesh->pnFaces[i] );
				face[i] = 0;
			}

			for(i = 0; i < iFace; i++) {
				int material = pFaces[i].material;
				for(j = 0; j < 3; j++) {
					pMesh->ppIndices[ material ][ 3 * face[ material ] + j ] = 
						pFaces[i].vertex[j];
				}
				face[ material ] += 1;
			}
			free(face);
		}
	}
	else
	// initialize lookup[ vertex ][ normal ] table
	{
		int nVertices = 0;
		int **lookup = malloc( sizeof(int*) * iVertex );

		for(i = 0; i < iVertex; i++)
		{
			lookup[i] = malloc( sizeof(int) * iNormal );
			for(j = 0; j < iNormal; j++)
			{
				lookup[i][j] = -1;
			}
		}
		assert(iType == QUAD_MESH);	  
		for(i = 0; i < iFace; i++) {
			for(j = 0; j < iFaceSize; j++) {
				int vertex = pqFaces[i].vertex[j] - 1;
				int normal = pqFaces[i].normal[j] - 1;
				if( lookup[ vertex ][ normal ] == -1 )
				{
					lookup[ vertex ][ normal ] = nVertices;
					nVertices++;
				}
			}
		}
	  
		// now that we know everything, build vertexarray based mesh
		// copy normals & vertices indexed by lookup-table
		pMesh->pVB = nebu_Mesh_VB_Create(NEBU_MESH_POSITION | NEBU_MESH_NORMAL, nVertices);
		for(i = 0; i < iVertex; i++) {
			for(j = 0; j < iNormal; j++) {
				int vertex = lookup[ i ][ j ];
				if(vertex != -1 ) {
					for(k = 0; k < 3; k++) {
						*(pMesh->pVB->pVertices + 3 * vertex + k) = pVertices[ i ].v[k];
						*(pMesh->pVB->pNormals + 3 * vertex + k) = pNormals[ j ].v[k];
					}
				}
			}
		}
		// build indices (per Material)
		{
			int *face;
			face = malloc( sizeof(int) * pMesh->nMaterials );
			pMesh->ppIndices = malloc( sizeof(GLshort*) * pMesh->nMaterials );
			for(i = 0; i < pMesh->nMaterials; i++) {
				pMesh->ppIndices[i] = 
				malloc( sizeof(GLshort) * iFaceSize * pMesh->pnFaces[i] );
				face[i] = 0;
			}

			assert(iType == QUAD_MESH);

			for(i = 0; i < iFace; i++) {
				int material = pqFaces[i].material;
				for(j = 0; j < iFaceSize; j++) {
					int vertex = pqFaces[i].vertex[j] - 1;
					int normal = pqFaces[i].normal[j] - 1;
					pMesh->ppIndices[ material ][ iFaceSize * face[ material ] + j ] = 
					lookup[ vertex ][ normal ];
				}
				face[ material ] = face[ material] + 1;
			}
			free(face);
		}
		// printf("[scenegraph] vertices: %d, faces: %d\n", nVertices, iFace);

		for(i = 0; i < iVertex; i++) {
			free(lookup[i]);
		}
		free(lookup);
	}
	free(pVertices);
	free(pNormals);

	switch(iType) {
	case TRI_MESH:
		free(pFaces);
		break;
	case QUAD_MESH:
		free(pqFaces);
		break;
	}
	gltron_Mesh_ComputeBBox(pMesh);

	return pMesh;
}

void gltron_Mesh_Draw(gltron_Mesh *pMesh, gltron_MeshType iType) {
  int i;
  int iFaceSize = 0;
  GLenum primitive = GL_TRIANGLES;

  switch(iType) {
  case TRI_MESH:
    primitive = GL_TRIANGLES;
    iFaceSize = 3;
    break;
  case QUAD_MESH:
    primitive = GL_QUADS;
    iFaceSize = 4;
    break;
  default:
    fprintf(stderr, "[fatal]: illegal mesh type\n");
    exit(1);
  }

  nebu_Mesh_VB_Enable(pMesh->pVB);

  for(i = 0; i < pMesh->nMaterials; i++) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
		 pMesh->pMaterials[i].ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
		 pMesh->pMaterials[i].diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
		 pMesh->pMaterials[i].specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
		pMesh->pMaterials[i].shininess);

    glDrawElements(primitive, iFaceSize * pMesh->pnFaces[i],
		   GL_UNSIGNED_SHORT, pMesh->ppIndices[i]);
  }
	nebu_Mesh_VB_Disable(pMesh->pVB);
}

void gltron_Mesh_DrawExplosion(gltron_Mesh *pMesh, float fRadius) {
  int i, j, k;
#define EXP_VECTORS 10
  float vectors[][3] = {
    { 0.03f, -0.06f, -0.07f }, 
    { 0.04f, 0.08f, -0.03f }, 
    { 0.10f, -0.04f, -0.07f }, 
    { 0.06f, -0.09f, -0.10f }, 
    { -0.03f, -0.05f, 0.02f }, 
    { 0.07f, 0.08f, -0.00f }, 
    { 0.01f, -0.04f, 0.10f }, 
    { -0.01f, -0.07f, 0.09f }, 
    { 0.01f, -0.01f, -0.09f }, 
    { -0.04f, 0.04f, 0.02f }
  };

  for(i = 0; i < pMesh->nMaterials; i++) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
        pMesh->pMaterials[i].ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
        pMesh->pMaterials[i].diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,
        pMesh->pMaterials[i].specular);
    glMaterialf(GL_FRONT, GL_SHININESS,
        pMesh->pMaterials[i].shininess);

    for(j = 0; j < pMesh->pnFaces[i]; j++) {
      float *normal, *vertex;

      normal = pMesh->pVB->pNormals + 3 * pMesh->ppIndices[i][3 * j];

      glPushMatrix();
      glTranslatef(fRadius * (*(normal + 0) + vectors[j % EXP_VECTORS][0]),
          fRadius * (*(normal + 1) + vectors[j % EXP_VECTORS][1]),
          fabsf(fRadius * (*(normal + 2) + vectors[j % EXP_VECTORS][2]) ));
      glBegin(GL_TRIANGLES);
      for(k = 0; k < 3; k++) {
        normal = pMesh->pVB->pNormals + 3 * pMesh->ppIndices[i][3 * j + k];
        vertex = pMesh->pVB->pVertices + 3 * pMesh->ppIndices[i][3 * j + k];

        glNormal3fv(normal);
        glVertex3fv(vertex);
      }
      glEnd();
      glPopMatrix();
    }
  }
}

void gltron_Mesh_ComputeBBox(gltron_Mesh *pMesh) {
  int i, j;
  vec3 vMin, vMax, vSize;

  vec3_Copy(&vMin, (vec3*) pMesh->pVB->pVertices);
  vec3_Copy(&vMax, (vec3*) pMesh->pVB->pVertices);

  for(i = 0; i < pMesh->pVB->nVertices; i++) {
    for(j = 0; j < 3; j++) {
      if(vMin.v[j] > pMesh->pVB->pVertices[3 * i + j])
				vMin.v[j] = pMesh->pVB->pVertices[3 * i + j];
      if(vMax.v[j] < pMesh->pVB->pVertices[3 * i + j])
				vMax.v[j] = pMesh->pVB->pVertices[3 * i + j];
    }
  }
  
  vec3_Sub(&vSize, &vMax, &vMin);
  vec3_Copy(&pMesh->BBox.vMin, &vMin);
  vec3_Copy(&pMesh->BBox.vSize, &vSize);
  pMesh->BBox.fRadius=vec3_Length(& pMesh->BBox.vSize)/10;
}
