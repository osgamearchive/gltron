#ifndef MODEL_H
#define MODEL_H

#define MODEL_USE_MATERIAL 1
#define MODEL_DRAW_BBOX    2

/* warning: changing this will break drawModel() */
#define MODEL_FACESIZE 4

typedef struct {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  char *name;
} Material;

typedef struct {
  int nFaces;
  int *facesizes;
  float *vertices;
  float *normals;
} MeshPart;

typedef struct {
  int nFaces;
  int nMaterials;
  Material *materials;
  MeshPart *meshparts;
  float bbox[3];
} Mesh;

extern char* getFullPath(char* filename);
extern int loadMaterials(char* filename, Material **materials);
extern Mesh* loadModel(const char *filename, float size, int flags);
extern void unloadModel(Mesh *mesh);
extern void drawModel(Mesh *mesh, int mode, int flag);
extern void drawExplosion(Mesh *mesh, float radius, int mode, int flag);
extern void setMaterialAlphas(Mesh *mesh, float alpha);
extern void setMaterialAmbient(Mesh *mesh, int material, float* color);
extern void setMaterialDiffuse(Mesh *mesh, int material, float* color);
extern void setMaterialSpecular(Mesh *mesh, int material, float* color);

#endif
