#ifndef NEBU_MESH_H
#define NEBU_MESH_H

typedef struct {
	int nVertices;
	float *pVertices;
	float *pNormals;
	float *pTexCoords;
	int vertexformat;

	int nTriangles;
	int *pTriangles;

} nebu_Mesh;

enum {
	NEBU_MESH_POSITION    = 0x0001,
	NEBU_MESH_NORMAL      = 0x0002,
	NEBU_MESH_TEXCOORD    = 0x0004,
	NEBU_MESH_FLAGS = 3
};

nebu_Mesh* nebu_Mesh_Create(int flags, int nVertices, int nTriangles);
nebu_Mesh* nebu_Mesh_Clone(int flags, nebu_Mesh *pMesh);
void nebu_Mesh_Free(nebu_Mesh *pMesh);
void nebu_Mesh_ComputeNormals(nebu_Mesh *pMesh);
void nebu_Mesh_ComputeTriangleNormal(nebu_Mesh *pMesh, int triangle, float* normal);
void nebu_Mesh_Scale(nebu_Mesh *pMesh, float fScale);
void nebu_Mesh_DrawGeometry(nebu_Mesh *pMesh);

#endif
