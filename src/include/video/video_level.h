#ifndef VIDEO_LEVEL_H
#define VIDEO_LEVEL_H

#include "base/nebu_vector.h"
#include "video/nebu_mesh.h"

typedef struct {
	int scalable;

	nebu_Mesh *floor;
	nebu_Mesh *arena;
} video_level;

void video_FreeLevel(video_level *l);
video_level* video_CreateLevel(void);
void video_ScaleLevel(video_level *l, float fSize);

#endif
