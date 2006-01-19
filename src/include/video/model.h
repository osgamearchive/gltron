#ifndef MODEL_H
#define MODEL_H

#define MAX_VERTICES 60000
#define MAX_FACES 120000
#define MAX_NORMALS 60000
#define MAX_MATERIALS 200

#define BUF_SIZE 120

#include "base/nebu_vector.h"
#include "video/nebu_mesh.h"

typedef struct gltron_Mesh_Material {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
  char *name;
  char *map_diffuse;
} gltron_Mesh_Material;

typedef struct gltron_Mesh_BoundingBox {
  vec3 vMin;
  vec3 vSize;
  float fRadius;
} gltron_Mesh_BoundingBox;

typedef struct gltron_Mesh {
	/*
	float *pVertices;
	float *pNormals;
	int nVertices, nNormals;
	*/
	nebu_Mesh_VB *pVB;
	nebu_Mesh_ShadowInfo *pSI;
	unsigned short **ppIndices;
	int *pnFaces;
	int nMaterials;
	gltron_Mesh_Material* pMaterials;
	gltron_Mesh_BoundingBox BBox;
} gltron_Mesh;

typedef enum gltron_MeshType {
  TRI_MESH,
  QUAD_MESH
} gltron_MeshType;
    
gltron_Mesh* gltron_Mesh_LoadFromFile(const char* filename, gltron_MeshType type);
void gltron_Mesh_Draw(gltron_Mesh* pMesh, gltron_MeshType type);
void gltron_Mesh_DrawExplosion(gltron_Mesh *pMesh, float fRadius);
void gltron_Mesh_Free(gltron_Mesh* pMesh);

typedef enum ColorType {
  eAmbient = 0,
  eDiffuse,
  eSpecular
} ColorType;

void gltron_Mesh_SetMaterialAlpha(gltron_Mesh *pMesh, float fAlpha);
void gltron_Mesh_SetMaterialColor(gltron_Mesh *pMesh, char *name, 
	 ColorType eType, float pColor[4]);

void gltron_Mesh_ComputeBBox(gltron_Mesh* pMesh);

#endif /* MODEL_H */
