#include "trail_geometry.h"

enum {
	COLOR_TRAIL, COLOR_BRIGHT, COLOR_CYCLE
};

void float2ubyte(unsigned char* pubColor, float *pfColor) {
	pubColor[0] = (unsigned char)(pfColor[0] * 255.0f);
	pubColor[1] = (unsigned char)(pfColor[1] * 255.0f);
	pubColor[2] = (unsigned char)(pfColor[2] * 255.0f);
	pubColor[3] = (unsigned char)(pfColor[3] * 255.0f);
}
		 
void storeColor(TrailMesh *pMesh, int offset, Player *p, int type) {
  float color[] = { 0, 0, 0, 1 };
	float white[] = { 1, 1, 1, 1 };

	switch(type) {
	case COLOR_TRAIL:
		memcpy(color, p->pColorAlpha, 3 * sizeof(float));
		break;
	case COLOR_BRIGHT:
		memcpy(color, white, 3 * sizeof(float));
		break;
	case COLOR_CYCLE:
		memcpy(color, p->pColorDiffuse, 3 * sizeof(float));
		break;
	}
	float2ubyte(pMesh->pColors + 4 * offset, color);
	float2ubyte(pMesh->pColors + 4 * (offset + 1), color);
}
		
void storeVertex(TrailMesh *pMesh, int offset, 
								 Line *line, int bUseEnd, 
								 float trail_height, float fSegLength, float fTotalLength) {
	vec3 *pVertices = pMesh->pVertices + offset;
	vec3 *pNormals = pMesh->pNormals + offset;
	vec2 *pTexCoords = pMesh->pTexCoords + offset;
	vec3 v;
	vec2 uv;
	float fUStart;
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

	fUStart = (fTotalLength / DECAL_WIDTH) - floorf(fTotalLength / DECAL_WIDTH);
	
	if(!bUseEnd) {
		v.v[0] = line->sx;
		v.v[1] = line->sy;
		v.v[2] = 0;
		uv.v[0] = fUStart;
	} else {
		v.v[0] = line->ex;
		v.v[1] = line->ey;
		v.v[2] = 0;
		uv.v[0] = fUStart + fSegLength / DECAL_WIDTH;
	}
	uv.v[1] = 0;
	vec3Copy(pVertices, &v);
	vec3Copy(pNormals, pvNormals + iNormal);
	vec2Copy(pTexCoords, &uv);
			
	v.v[2] = trail_height;
	uv.v[1] = 1;
	vec3Copy(pVertices + 1, &v);
	vec3Copy(pNormals + 1, pvNormals + iNormal);
	vec2Copy(pTexCoords + 1, &uv);
}

void storeIndices(TrailMesh *pMesh, int indexOffset, int vertexOffset) {
	unsigned short ppBase[2][6] = {
		{ 0, 2, 1, 2, 3, 1 },
		{ 0, 1, 2, 1, 3, 2 }
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
	
	for(i = 0; i < 6; i++) {
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
	float fTotalLength = 0;
	float fSegLength;
	for(i = 0; i < pData->trailOffset; i++) {
		fSegLength = 
			fabs( (pData->trails + i)->ex - (pData->trails + i)->sx) +
			fabs( (pData->trails + i)->ey - (pData->trails + i)->sy);
		if(i == 0 || cmpdir(pData->trails + i - 1, pData->trails + i)) {
			storeVertex(pMesh, curVertex, pData->trails + i, 0, 
									pData->trail_height, fSegLength, fTotalLength);
			storeColor(pMesh, curVertex, pPlayer, COLOR_TRAIL);
			curVertex += 2;
		}
			
		storeVertex(pMesh, curVertex, pData->trails + i, 1,
								pData->trail_height, fSegLength, fTotalLength);
		storeColor(pMesh, curVertex, pPlayer, COLOR_TRAIL);
		curVertex += 2;

		storeIndices(pMesh, curIndex, curVertex - 4);
		curIndex += 6;

		fTotalLength += fSegLength;

	}
	{
		Line line;
		line.sx = pData->trails[pData->trailOffset].sx;
		line.sy = pData->trails[pData->trailOffset].sy;
		line.ex = getSegmentEndX( pData, 1 );
		line.ey = getSegmentEndY( pData, 1 );

		fSegLength = 
			fabs( line.ex - line.sx) +
			fabs( line.ey - line.sy);
		
		storeVertex(pMesh, curVertex, &line, 0,
								pData->trail_height, fSegLength, fTotalLength);
		storeColor(pMesh, curVertex, pPlayer, COLOR_TRAIL);
		curVertex += 2;
		
		storeVertex(pMesh, curVertex, &line, 1,
								pData->trail_height, fSegLength, fTotalLength);
		storeColor(pMesh, curVertex, pPlayer, COLOR_TRAIL);
		curVertex += 2;

		storeIndices(pMesh, curIndex, curVertex - 4);
		curIndex += 6;

		fTotalLength += fSegLength;

		line.sx = line.ex;
		line.sy = line.ey;
		line.ex = getSegmentEndX( pData, 0 );
		line.ey = getSegmentEndY( pData, 0 );

		fSegLength = 
			fabs( line.ex - line.sx) +
			fabs( line.ey - line.sy);

		storeVertex(pMesh, curVertex, &line, 0,
								pData->trail_height, fSegLength, fTotalLength);
		storeColor(pMesh, curVertex, pPlayer, COLOR_TRAIL);
		curVertex += 2;

		storeVertex(pMesh, curVertex, &line, 1,
								pData->trail_height, fSegLength, fTotalLength);
		storeColor(pMesh, curVertex, pPlayer, COLOR_BRIGHT);
		curVertex += 2;

		storeIndices(pMesh, curIndex, curVertex - 4);
		curIndex += 6;
	}

	pMesh->iUsed = curIndex;
}

void bowGeometry(Player *pPlayer, TrailMesh *pMesh, int vOffset, int iOffset) {
	Data *pData = pPlayer->data;
	Line line;
	int bdist = PLAYER_IS_ACTIVE(pPlayer) ? 2 : 3;

	line.sx = getSegmentEndX( pData, 0 );
	line.sy = getSegmentEndY( pData, 0 );
	line.ex = getSegmentEndX( pData, bdist );
	line.ey = getSegmentEndY( pData, bdist );
	storeVertex(pMesh, vOffset + 0, &line, 0, 
							pData->trail_height, DECAL_WIDTH, 0);
	storeColor(pMesh, vOffset + 0, pPlayer, COLOR_BRIGHT);
	storeVertex(pMesh, vOffset + 2, &line, 1, 
							pData->trail_height, DECAL_WIDTH, 0);
	storeColor(pMesh, vOffset + 2, pPlayer, COLOR_CYCLE);

	storeIndices(pMesh, iOffset, vOffset);
	pMesh->iUsed = 6;
}
	
	
	
