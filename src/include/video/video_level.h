#ifndef VIDEO_LEVEL_H
#define VIDEO_LEVEL_H

#include "video/nebu_mesh.h"

typedef struct video_level_shader {
	int lit;
	int diffuse_texture_id;
} video_level_shader;

typedef struct video_level {
	int scalable;

	nebu_Mesh *floor;
	nebu_Mesh *arena;

	video_level_shader floor_shader;
	video_level_shader arena_shader;
} video_level;

void video_LoadLevel(void);
void video_FreeLevel(video_level *l);
video_level* video_CreateLevel(void);
void video_ScaleLevel(video_level *l, float fSize);
void video_Shader_Setup(video_level_shader *shader);
void video_Shader_Cleanup(video_level_shader *shader);
#endif
