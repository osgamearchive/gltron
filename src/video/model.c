// include a few datastructures & constants

#include "video/model.h"
#include "video/video.h"

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "zlib.h"

extern void readMaterialLibrary(char *buf, Mesh *pMesh);
extern void setMaterial(char *buf, Mesh *pMesh, int *iGroup);

void readVector(char *buf, vec3 *pVertex ) {
  if( sscanf(buf, " %f %f %f ", 
	     pVertex->v, pVertex->v + 1, pVertex->v + 2) != 3) {
    fprintf(stderr, "*** failed reading Vector from %s\n", buf);
  }
}

void readTriFace(char *buf, face *pFace, int *iFace, int iGroup) {
  // assumes model in "f %d//%d %d//%d %d//%d\n" format
  if(
     sscanf(buf, "f %d//%d %d//%d %d//%d ",
	    pFace[ *iFace ].vertex + 0, pFace[ *iFace ].normal + 0,
	    pFace[ *iFace ].vertex + 1, pFace[ *iFace ].normal + 1,
	    pFace[ *iFace ].vertex + 2, pFace[ *iFace ].normal + 2) == 6) {
    pFace[ *iFace ].material = iGroup;
    (*iFace)++;
  } else {
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

Mesh* readMeshFromFile(const char *filename, MeshType iType) {
  // allocate some buffers
  // vertices, normals

  vec3 *pVertices = malloc( sizeof(vec3) * MAX_VERTICES );
  vec3 *pNormals = malloc( sizeof(vec3) * MAX_NORMALS );

  quadFace *pqFaces = NULL;
  face *pFaces = NULL;
  int iFaceSize = 0;

  Mesh *pMesh = malloc( sizeof(Mesh) );
  int iGroup = 0;
  int iVertex = 0, iNormal = 0, iFace = 0;

  gzFile f;
  char buf[BUF_SIZE];

  int i, j, k;


  switch(iType) {
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

  if((f = gzopen(filename, "r")) == 0) {
    fprintf(stderr, "*** could not open file '%s'\n", filename);
    return NULL;
  }

  while( gzgets(f, buf, sizeof(buf)) ) {
    switch(buf[0]) {
    case 'm': // material library
      readMaterialLibrary(buf, pMesh);
      break;
    case 'u': // material  name
      setMaterial(buf, pMesh, &iGroup);
      break;
    case 'v': // vertex, normal, texture coordinate
      switch(buf[1]) {
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
      switch(iType) {
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
    for(i = 0; i < iFace; i++) {
      pMesh->pnFaces[ pFaces[i].material ] += 1;
    }
    break;
  case QUAD_MESH:
    for(i = 0; i < iFace; i++) {
      pMesh->pnFaces[ pqFaces[i].material ] += 1;
    }
    break;
  }


  // combine vectors & normals for each vertex, doubling where necessary

  // initialize lookup[ vertex ][ normal ] table
  {
    int nVertices = 0;
    int **lookup = malloc( sizeof(int*) * iVertex );

    for(i = 0; i < iVertex; i++) {
      lookup[i] = malloc( sizeof(int) * iNormal );
      for(j = 0; j < iNormal; j++) {
	lookup[i][j] = -1;
      }
    }
  
    switch(iType) {
    case TRI_MESH:
      for(i = 0; i < iFace; i++) {
	for(j = 0; j < iFaceSize; j++) {
	  int vertex = pFaces[i].vertex[j] - 1;
	  int normal = pFaces[i].normal[j] - 1;
	  if( lookup[ vertex ][ normal ] == -1 ) {
	    lookup[ vertex ][ normal ] = nVertices;
	    nVertices++;
	  }
	}
      }
      break;
    case QUAD_MESH:
      for(i = 0; i < iFace; i++) {
	for(j = 0; j < iFaceSize; j++) {
	  int vertex = pqFaces[i].vertex[j] - 1;
	  int normal = pqFaces[i].normal[j] - 1;
	  if( lookup[ vertex ][ normal ] == -1 ) {
	    lookup[ vertex ][ normal ] = nVertices;
	    nVertices++;
	  }
	}
      }
      break;
    }
  
    // now that we know everything, build vertexarray based mesh
    // copy normals & vertices indexed by lookup-table
    pMesh->nVertices = nVertices;
    pMesh->pVertices = malloc( sizeof(GLfloat) * 3 * nVertices );
    pMesh->pNormals = malloc( sizeof(GLfloat) * 3 * nVertices );
    for(i = 0; i < iVertex; i++) {
      for(j = 0; j < iNormal; j++) {
	int vertex = lookup[ i ][ j ];
	if(vertex != -1 ) {
	  for(k = 0; k < 3; k++) {
	    *(pMesh->pVertices + 3 * vertex + k) = pVertices[ i ].v[k];
	    *(pMesh->pNormals + 3 * vertex + k) = pNormals[ j ].v[k];
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
      switch(iType) {
      case TRI_MESH:
	for(i = 0; i < iFace; i++) {
	  int material = pFaces[i].material;
	  for(j = 0; j < iFaceSize; j++) {
	    int vertex = pFaces[i].vertex[j] - 1;
	    int normal = pFaces[i].normal[j] - 1;
	    pMesh->ppIndices[ material ][ iFaceSize * face[ material ] + j ] = 
	      lookup[ vertex ][ normal ];
	  }
	  face[ material ] = face[ material] + 1;
	}
	break;
      case QUAD_MESH:
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
	break;
      }
      free(face);
    }
    // printf("[scenegraph] vertices: %d, faces: %d\n", nVertices, iFace);

    free(lookup);
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
  }
  computeBBox(pMesh);

  return pMesh;
}

void drawModel(Mesh *pMesh, MeshType iType) {
  int i;
  int iFaceSize = 0;
  GLenum primitive = GL_TRIANGLES;

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, pMesh->pVertices);
  glNormalPointer(GL_FLOAT, 0, pMesh->pNormals);

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

  for(i = 0; i < pMesh->nMaterials; i++) {
    glMaterialfv(GL_FRONT, GL_AMBIENT,
		 pMesh->pMaterials[i].ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,
		 pMesh->pMaterials[i].diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,
		 pMesh->pMaterials[i].specular);
    glMaterialf(GL_FRONT, GL_SHININESS,
		pMesh->pMaterials[i].shininess);

    glDrawElements(primitive, iFaceSize * pMesh->pnFaces[i],
		   GL_UNSIGNED_SHORT, pMesh->ppIndices[i]);

    polycount += pMesh->pnFaces[i];
  }

}

void drawModelExplosion(Mesh *pMesh, float fRadius) {
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

      normal = pMesh->pNormals + 3 * pMesh->ppIndices[i][3 * j];

      glPushMatrix();
      glTranslatef(fRadius * (*(normal + 0) + vectors[j % EXP_VECTORS][0]),
          fRadius * (*(normal + 1) + vectors[j % EXP_VECTORS][1]),
          fabsf(fRadius * (*(normal + 2) + vectors[j % EXP_VECTORS][2]) ));
      glBegin(GL_TRIANGLES);
      for(k = 0; k < 3; k++) {
        normal = pMesh->pNormals + 3 * pMesh->ppIndices[i][3 * j + k];
        vertex = pMesh->pVertices + 3 * pMesh->ppIndices[i][3 * j + k];

        glNormal3fv(normal);
        glVertex3fv(vertex);
      }
      glEnd();
      glPopMatrix();
    }
    polycount += pMesh->pnFaces[i];
  }
}

void computeBBox(Mesh *pMesh) {
  int i, j;
  vec3 vMin, vMax, vSize;

  vcopy(pMesh->pVertices, vMin.v);
  vcopy(pMesh->pVertices, vMax.v);

  for(i = 0; i < pMesh->nVertices; i++) {
    for(j = 0; j < 3; j++) {
      if(vMin.v[j] > pMesh->pVertices[3 * i + j])
	vMin.v[j] = pMesh->pVertices[3 * i + j];
      if(vMax.v[j] < pMesh->pVertices[3 * i + j])
	vMax.v[j] = pMesh->pVertices[3 * i + j];
    }
    /*
    if(
       vMin.v[0] <= pMesh->pVertices[3 * i + 0] &&
       vMin.v[1] <= pMesh->pVertices[3 * i + 1] &&
       vMin.v[2] <= pMesh->pVertices[3 * i + 2]
       )
      vcopy(pMesh->pVertices + 3 * i, vMin.v);
    if(
       vMax.v[0] >= pMesh->pVertices[3 * i + 0] &&
       vMax.v[1] >= pMesh->pVertices[3 * i + 1] &&
       vMax.v[2] >= pMesh->pVertices[3 * i + 2]
       )
      vcopy(pMesh->pVertices + 3 * i, vMax.v);
    */
  }
  
  vsub(vMax.v, vMin.v, vSize.v);
  vcopy(vMin.v, pMesh->BBox.vMin.v);
  vcopy(vSize.v, pMesh->BBox.vSize.v);
  pMesh->BBox.fRadius=length(pMesh->BBox.vSize.v)/10;
}
