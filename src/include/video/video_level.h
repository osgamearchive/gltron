#ifndef VIDEO_LEVEL_H
#define VIDEO_LEVEL_H

#include "base/nebu_vector.h"
#include "base/nebu_mesh.h"

typedef struct {
	int scalable;

	mesh *floor;
	mesh *arena;
} video_level;

void video_free_level(video_level *l);
video_level video_create_level(const char *name);

#endif
