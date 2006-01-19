#include "video/video.h"
#include "game/game.h"
#include "video/nebu_renderer_gl.h"
#include "game/resource.h"
#include "video/graphics_lights.h"

void drawWalls(void) {
	// DEBUG
	gltron_Mesh *arena = (gltron_Mesh*)resource_Get(gpTokenCurrentLevel, eRT_GLtronTriMesh);

	
	setupLights(eCyclesWorld);
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(arena->BBox.vSize.v[0] / 2, arena->BBox.vSize.v[1] / 2, arena->BBox.vSize.v[2] / 2);
	
	gltron_Mesh_Draw(arena, TRI_MESH);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	// */ 
	/* 
	video_Shader_Setup(& gWorld->arena_shader);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	nebu_Mesh_DrawGeometry(gWorld->arena);
	video_Shader_Cleanup(& gWorld->arena_shader);

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// */
}

