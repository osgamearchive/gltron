#include "video/video.h"
#include "game/game.h"
#include "video/nebu_renderer_gl.h"

void drawWalls(void) {
	video_Shader_Setup(& gWorld->arena_shader);

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	nebu_Mesh_DrawGeometry(gWorld->arena);
	video_Shader_Cleanup(& gWorld->arena_shader);

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
}

