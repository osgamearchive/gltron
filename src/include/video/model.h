#ifndef MODEL_H
#define MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VERTICES 30000
#define MAX_FACES 20000
#define MAX_NORMALS 30000
#define MAX_MATERIALS 200

#define BUF_SIZE 120

#define NO_SDL_GLEXT
#include "SDL_opengl.h"
// #include <GL/gl.h>

#include "Nebu_base.h"
#include "Nebu_filesystem.h"

typedef struct {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
  char *name;
  char *map_diffuse;
} gltron_Mesh_Material;

typedef struct {
  vec3 vMin;
  vec3 vSize;
  float fRadius;
} gltron_Mesh_BoundingBox;

typedef struct {
  GLfloat *pVertices;
  GLfloat *pNormals;
  GLshort **ppIndices;
  int nVertices, nNormals, nMaterials;
  gltron_Mesh_Material* pMaterials;
  int *pnFaces;
  gltron_Mesh_BoundingBox BBox;
} gltron_Mesh;

typedef enum {
  TRI_MESH,
  QUAD_MESH
} gltron_MeshType;
    
extern gltron_Mesh* gltron_Mesh_LoadFromFile(const char* filename, gltron_MeshType type);
extern void gltron_Mesh_Draw(gltron_Mesh* pMesh, gltron_MeshType type);
extern void gltron_Mesh_DrawExplosion(gltron_Mesh *pMesh, float fRadius);

typedef enum {
  eAmbient = 0,
  eDiffuse,
  eSpecular
} ColorType;

extern void gltron_Mesh_SetMaterialColor(gltron_Mesh *pMesh, char *name, 
																				 ColorType eType, float pColor[4]);

extern void gltron_Mesh_ComputeBBox(gltron_Mesh* pMesh);

extern int polycount;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MODEL_H */
