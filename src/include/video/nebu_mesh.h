#ifndef VIDEO_MESH_H
#define VIDEO_MESH_H

enum { 
	ePosition = 0,
	eNormal,
	eColor0,
	eColor1,
	eColor2,
	eTexCoord0,
	eTexCoord1,
	eTexCoord2,
	eTexCoord3,
	eTexCoord4,
	eTexCoord5,
	eTexCoord6,
	eTexCoord7,
	eTangent,
	eBiTangent,
	eMaxVertexFormat
};

int vertexSize[eMaxVertexFormat] = {
	12, 12, // normal, position (3 floats)
	4, 4, 4, // colors (dword)
	8, 8, 8, 8, 8, 8, 8, 8, // texcoords (2 floats)
	12, 12,	 // tangent space  (3 floats)
};

typedef struct {
	int vertexFormat;
	int primitiveType;
	
	int nVertices;
	void *pVertices[16];
	void *pPackedVertices;
	
	int nFaces;
	void *pIndices;
} mesh;

mesh *mesh_create(int format, int nVertices, int nFaces) {
	int i;
	
	mesh *m = malloc( sizeof(mesh) );
	
	m->format = format;
	m->nVertices = nVertices;
	m->nFaces = nFaces;
	
	for(i = 0; i < eMaxVertexFormat; i++) {
		if(format & (1 << i)) {
			m->pVertices[i] = malloc( vertexSize[i] * nVertices );
		} else {
			m->pVertices[i] = NULL;
		}
	}
	m->pIndices = malloc( 
}
#endif
