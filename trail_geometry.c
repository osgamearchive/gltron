#include "trail_geometry.h"

void storeVertex(TrailMesh *pMesh, int offset, 
								 Line *line, int bUseEnd, int trail_height) {
	vec3 *pVertices = pMesh->pVertices + offset;
	vec3 *pNormals = pMesh->pNormals + offset;
	vec3 v;
	vec3 pvNormals[] = {
		{ { 1, 0, 0 } },
		{ { -1, 0, 0 } },
		{ { 0, 1, 0 } },
		{ { 0, -1, 0 } }
	};
	
	int iNormal;
	if(line->sx == line->ex)
		// iNormal = (line->sx <= line->ex) ? 0 : 1;
		iNormal = 0;
	else
		// iNormal = (line->sy <= line->sy) ? 2 : 3;
		iNormal = 2;
	
	if(!bUseEnd) {
		v.v[0] = line->sx;
		v.v[1] = line->sy;
		v.v[2] = 0;
	} else {
		v.v[0] = line->ex;
		v.v[1] = line->ey;
		v.v[2] = 0;
	}
	vec3Copy(pVertices, &v);
	vec3Copy(pNormals, pvNormals + iNormal);
			
	v.v[2] = trail_height;
	vec3Copy(pVertices + 1, &v);
	vec3Copy(pNormals + 1, pvNormals + iNormal);
}

void storeIndices(TrailMesh *pMesh, int indexOffset, int vertexOffset) {
	unsigned short ppBase[2][4] = {
		{ 0, 2, 3, 1 },
		{ 0, 1, 3, 2 }
	};
	int i;
	int winding;

	if(pMesh->pVertices[vertexOffset].v[0] ==
		 pMesh->pVertices[vertexOffset + 2].v[0])
		winding = (pMesh->pVertices[vertexOffset].v[1] <= 
							 pMesh->pVertices[vertexOffset + 2].v[1]) ? 0 : 1;
	else
		winding = (pMesh->pVertices[vertexOffset].v[0] < 
							 pMesh->pVertices[vertexOffset + 2].v[0]) ? 1 : 0;
	
	for(i = 0; i < 4; i++) {
		pMesh->pIndices[i + indexOffset] = ppBase[winding][i] + vertexOffset;
	}
}

int cmpdir(Line *line1, Line *line2) {
	if((line1->ex - line1->sx == 0 && line2->ex - line2->sx == 0) ||
		 (line1->ey - line1->sy == 0 && line2->ey - line2->sy == 0))
		return 0;
	return 1;
}

void trailGeometry(Player *pPlayer, TrailMesh *pMesh) {
	Data *pData = pPlayer->data;
	int curVertex = 0, curIndex = 0;
	int i;

	for(i = 0; i < pData->trailOffset; i++) {
		if(i == 0 || cmpdir(pData->trails + i - 1, pData->trails + i)) {
			storeVertex(pMesh, curVertex, pData->trails + i, 0, pData->trail_height);
			curVertex += 2;
		}
		storeVertex(pMesh, curVertex, pData->trails + i, 1,
								pData->trail_height);
		storeIndices(pMesh, curIndex, curVertex - 2);
		curVertex += 2;
		curIndex += 4;
	}
	{
		Line line;
		line.sx = pData->trails[pData->trailOffset].sx;
		line.sy = pData->trails[pData->trailOffset].sy;
		line.ex = pData->iposx;
		line.ey = pData->iposy;
		
		storeVertex(pMesh, curVertex, &line, 0,
								pData->trail_height);
		curVertex += 2;

		storeVertex(pMesh, curVertex, &line, 1,
								pData->trail_height);
		storeIndices(pMesh, curIndex, curVertex - 2);
		curVertex += 2;
		curIndex += 4;
	}

	pMesh->iUsed = curIndex;
}
								 
void trailRender(Player *pPlayer, TrailMesh *pMesh) {
	if(pMesh->iUsed == 0)
		return;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glDisable(GL_LIGHTING);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pMesh->pVertices);
	glNormalPointer(GL_FLOAT, 0, pMesh->pNormals);

	// glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	{ 
		float black[] = { 0, 0, 0, 1 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
								 pPlayer->pColorAlpha);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
	}
	
	// glCullFace(GL_BACK);
	glDrawElements(GL_QUADS, pMesh->iUsed, GL_UNSIGNED_SHORT, pMesh->pIndices);

#if 0
	{ 
		float color[] = { 1, .2, .2, 1 }; 
		float black[] = { 0, 0, 0, 1 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
	}

	glCullFace(GL_FRONT);
	glDrawElements(GL_QUADS, pMesh->iUsed, GL_UNSIGNED_SHORT, pMesh->pIndices);
#endif

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	
	// glEnable(GL_LIGHTING);
	checkGLError("trail");
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
