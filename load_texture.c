#include "gltron.h"

void freeTextureData(texture *tex) {
  free(tex->data);
  free(tex);
}

texture* loadTextureData(char *filename) {
  texture *tex;
  char *path;

  path = getFullPath(filename);
  if(path != NULL)
    tex = LOAD_TEX(path);
  else {
    fprintf(stderr, "fatal: could not load %s, exiting...\n", filename);
    exit(1);
  }

  if(tex == NULL) {    
    fprintf(stderr, "fatal: failed load %s, exiting...\n", filename);
    exit(1);
  }
  return tex;
}

void loadTexture(char *filename, int format) {
  texture *tex;
  GLint internal;

  tex = loadTextureData(filename);
  if(tex->channels == 3) internal = GL_RGB;
  else internal = GL_RGBA;
  /* TODO: build mipmaps the proper way, box filters suck */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if(game->settings->use_mipmaps) {
    gluBuild2DMipmaps(GL_TEXTURE_2D, format, tex->width, tex->height, 
		      internal, GL_UNSIGNED_BYTE, tex->data);
  } else { 
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0,
		 internal, GL_UNSIGNED_BYTE, tex->data);
  }
  freeTextureData(tex);
}
