#ifndef NEBU_SURFACE_H
#define NEBU_SURFACE_H

#include "video/nebu_png_texture.h"

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

nebu_Surface* nebu_Surface_LoadPNG(const char *filename) {
	nebu_Surface *pSurface;

	/* use old code to load texture, do format conversion */
	/* TODO: clean this up, throw away old format */
	
	png_texture *pTex;
	pTex = load_png_texture(filename);
	if(!pTex)
		return NULL;

	pSurface = (nebu_Surface*) malloc( sizeof(nebu_Surface) );
	pSurface->w = pTex->width;
	pSurface->h = pTex->height;
	switch(pTex->channels) {
	case 1: pSurface->format = NEBU_SURFACE_ALPHA; break;
	case 3: pSurface->format = NEBU_SURFACE_RGB; break;
	case 4: pSurface->format = NEBU_SURFACE_RGBA; break;
	default: pSurface->format = NEBU_SURFACE_UNKNOWN; break;
	}
	pSurface->data = pTex->data;
	free(pTex);
	return pSurface;
}
#endif
