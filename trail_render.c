#include "trail_geometry.h"
								 
void trailRender(Player *pPlayer, TrailMesh *pMesh, int texture) {
	if(pMesh->iUsed == 0)
		return;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glDisable(GL_LIGHTING);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, 0, pMesh->pVertices);
	glNormalPointer(GL_FLOAT, 0, pMesh->pNormals);
	glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pTexCoords);
	glColorPointer(4, GL_FLOAT, 0, pMesh->pColors);

	checkGLError("texcoord pointer");
	// glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	// glDisable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	{ 
		float black[] = { 0, 0, 0, 1 };
		float color[] = { 0, 0, 0, 1 };
		memcpy(color, pPlayer->pColorAlpha, 3 * sizeof(float));
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
	}

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
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

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_COLOR_MATERIAL);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	// glEnable(GL_LIGHTING);
	checkGLError("trail");
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
