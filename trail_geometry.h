#ifndef TRAIL_GEOMETRY_H
#define TRAIL_GEOMETRY_H

#include "gltron.h"

typedef struct {
	vec3* pVertices;
	vec3* pNormals;
	vec2* pTexCoords;
	unsigned short *pIndices;
	unsigned char *pColors;
	unsigned int iSize;
	unsigned int iUsed;
} TrailMesh;

void storeVertex(TrailMesh *pMesh, int offset, 
								 Line *line, int bUseEnd, 
								 float trail_height, float fSegLength, float fTotalLength);
void storeIndices(TrailMesh *pMesh, int indexOffset, int vertexOffset);
int cmpdir(Line *line1, Line *line2);
void trailGeometry(Player *pPlayer, TrailMesh *pMesh);
void bowGeometry(Player *pPlayer, TrailMesh *pMesh,
								 int vertexOffset, int indexOffset);
void trailRender(Player *pPlayer, TrailMesh *pMesh, int texture);

#endif
