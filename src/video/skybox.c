#include "video/video.h"
#include "game/game.h"

static void bindSkyboxTexture(int index) {
	glBindTexture(GL_TEXTURE_2D, gScreen->textures[ TEX_SKYBOX0 + index ]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void drawSkybox(float d) {  
	/* these are the values for y == up, x == front */
	/* 
	float sides[6][4][3] = {
		{ { 1, -1, -1 }, { 1, -1, 1 }, { 1, 1, 1 },  { 1, 1, -1 } }, // front
		{ { 1, 1, -1 }, { 1, 1, 1 }, { -1, 1, 1 }, { -1, 1, -1 } }, // top
		{ { -1, -1, -1 }, { 1, -1, -1 },  { 1, 1, -1 }, { -1, 1, -1 } }, // left
		{ { 1, -1, 1 }, { -1, -1, 1 }, { -1, 1, 1 }, { 1, 1, 1 } }, // right
		{ { -1, -1, -1 }, { 1, -1, -1 }, { 1, -1, 1 }, { -1, -1, 1 } }, // bottom
		{ { -1, -1, 1 }, { -1, -1, -1 }, { -1, 1, -1 }, { -1, 1, a1 } } // back
		};
	*/

	/* these values are for z == up, x == front */
	float sides[6][4][3] = {
		{ { 1, 1, -1 }, { 1, -1, -1 }, { 1, -1, 1 },  { 1, 1, 1 } }, /* front */
		{ { 1, 1, 1 }, { -1, 1, 1 }, { -1, -1, 1 }, { 1, -1, 1 } }, /* top */
		{ { -1, 1, -1 }, { 1, 1, -1 },  { 1, 1, 1 }, { -1, 1, 1 } }, /* left */
		{ { 1, -1, -1 }, { -1, -1, -1 }, { -1, -1, 1 }, { 1, -1, 1 } }, /* right */
		{ { -1, 1, -1 }, { -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 } }, /* bottom */
		{ { -1, -1, -1 }, { -1, 1, -1 }, { -1, 1, 1 }, { -1, -1, 1 } } /* back */
	};

	float uv[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
	int i, j;

	if (!gSettingsCache.show_skybox)
		return;

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);
	for(i = 0; i < 6; i++) {
		bindSkyboxTexture(i);
		glBegin(GL_QUADS);
		for(j = 0; j < 4; j++) {
			glTexCoord2fv( uv[j] );
			glVertex3f( sides[i][j][0] * d, sides[i][j][1] * d, sides[i][j][2] * d );
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}
