#include "gltron.h"

typedef struct {
	vec3* pVertices;
	vec3* pNormals;
	vec2* pTexCoords;
	unsigned int *pColors;
} TrailMesh;
	
void storeVertex(vec3 *pVertices, line *line, int trail_height) {
		vec3 v;
		v.v[0] = line->sx;
		v.v[1] = line->sy;
		v.v[2] = 0;
		
		vec3Copy(pVertices[cur], &v);
		
		v.v[2] = trail_height;
		vec3Copy(pVertices[cur + 3], &v);

		v.v[0] = line->ex;
		v.v[1] = line->ey;
		v.v[2] = 0;
		
		vec3Copy(pVertices[cur + 1], &v);
		
		v.v[2] = trail_height;
		vec3Copy(pVertices[cur + 2], &v);
}	
void trailGeometry(Player *pPlayer, TrailMesh *pMesh) {
	Data *pData = *pPlayer->data;
pp	int cur = 0;
	vec3* pVertices = pMesh->pVertices;
	vec3* pNormals = pMesh->pNormals;
	vec2* pTexCoords = pMesh->pTexCoords;
	
	for(int i = 0; i < pData->TrailCount; i++) {
		storeVertex(pVertices + cur, pData->trails + i,
								pData->trail_height);
		for(j = 0; j < 4; j++) {
		}
		storeNormals(pNormals + cur);
	}
}
								 
								
		
		
		

	
