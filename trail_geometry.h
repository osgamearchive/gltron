#ifndef TRAIL_GEOMETRY_H
#define TRAIL_GEOMETRY_H

#include "gltron.h"

typedef struct {
	vec3* pVertices;
	vec3* pNormals;
	vec2* pTexCoords;
	unsigned short *pIndices;
	unsigned int *pColors;
	unsigned int iSize;
	unsigned int iUsed;
} TrailMesh;

void storeVertex(TrailMesh *pMesh, int offset, 
								 Line *line, int bUseEnd, int trail_height);
void storeIndices(unsigned short *pIndices, int offset);
int cmpdir(Line *line1, Line *line2);
void trailGeometry(Player *pPlayer, TrailMesh *pMesh);
void trailRender(TrailMesh *pMesh);

#endif
