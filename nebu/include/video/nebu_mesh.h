#ifndef NEBU_MESH_H
#define NEBU_MESH_H

enum {
	NEBU_MESH_POSITION    = 0x0001,
	NEBU_MESH_NORMAL      = 0x0002,
	NEBU_MESH_TEXCOORD0   = 0x0004,
	NEBU_MESH_TEXCOORD1   = 0x0008,
	NEBU_MESH_TEXCOORD2   = 0x0010,
	NEBU_MESH_TEXCOORD3   = 0x0020,
	NEBU_MESH_TEXCOORD4   = 0x0040,
	NEBU_MESH_TEXCOORD5   = 0x0080,
	NEBU_MESH_TEXCOORD6   = 0x0100,
	NEBU_MESH_TEXCOORD7   = 0x0200,
	NEBU_MESH_TEXCOORD_MAXCOUNT = 8,

	NEBU_MESH_FLAGS = 10
};


typedef struct {
	int nVertices;
	float *pVertices;
	float *pNormals;
	float *pTexCoords[NEBU_MESH_TEXCOORD_MAXCOUNT];
	int vertexformat;

	int nTriangles;
	int *pTriangles;

} nebu_Mesh;


nebu_Mesh* nebu_Mesh_Create(int flags, int nVertices, int nTriangles);
nebu_Mesh* nebu_Mesh_Clone(int flags, nebu_Mesh *pMesh);
void nebu_Mesh_Free(nebu_Mesh *pMesh);
void nebu_Mesh_ComputeNormals(nebu_Mesh *pMesh);
void nebu_Mesh_ComputeTriangleNormal(nebu_Mesh *pMesh, int triangle, float* normal);
void nebu_Mesh_Scale(nebu_Mesh *pMesh, float fScale);
void nebu_Mesh_DrawGeometry(nebu_Mesh *pMesh);

#endif
