#ifndef NEBU_SURFACE_H
#define NEBU_SURFACE_H

#include "base/nebu_png_texture.h"

#include <stdlib.h>

typedef struct {
	int w, h;
	int format;
	unsigned char* data;
} nebu_Surface;

enum { 
	NEBU_SURFACE_RGB,
	NEBU_SURFACE_RGBA,
	NEBU_SURFACE_ALPHA,
	NEBU_SURFACE_UNKNOWN
};

nebu_Surface* nebu_Surface_LoadPNG(const char *filename);
void nebu_Surface_Free(nebu_Surface* pSurface);
#endif
