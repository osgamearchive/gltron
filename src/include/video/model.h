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
} Material;

typedef struct {
  vec3 vMin;
  vec3 vSize;
  float fRadius;
} BoundingBox;

typedef struct {
  GLfloat *pVertices;
  GLfloat *pNormals;
  GLshort **ppIndices;
  int nVertices, nNormals, nMaterials;
  Material* pMaterials;
  int *pnFaces;
  BoundingBox BBox;
} Mesh;

typedef enum {
  TRI_MESH,
  QUAD_MESH
} MeshType;
    
extern Mesh* readMeshFromFile(const char* filename, MeshType type);
extern void drawModel(Mesh* pMesh, MeshType type);
extern void drawModelExplosion(Mesh *pMesh, float fRadius);

typedef enum {
  eAmbient = 0,
  eDiffuse,
  eSpecular
} ColorType;

extern void SetMaterialColor(Mesh *pMesh, char *name, ColorType eType,
			     float pColor[4]);

extern void computeBBox(Mesh* pMesh);

extern int polycount;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MODEL_H */
