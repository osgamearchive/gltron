#include "gltron.h"

void freeTextureData(texture *tex) {
  free(tex->data);
  free(tex);
}

texture* loadTextureData(char *filename) {
  texture *tex = NULL;
  char *path;
  char *artpack;
  
  artpack = artpack_list[artpack_index];

  path = getArtPath(artpack, filename);
  if(path != NULL) {
    tex = LOAD_TEX(path);
    free(path);
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
  if(format == GL_DONT_CARE) {
    if(tex->channels == 3) format = GL_RGB;
    if(tex->channels == 4) format = GL_RGBA;
  }
  /* TODO: build mipmaps the proper way, box filters suck */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if(getSettingi("use_mipmaps")) {
    texture *newtex;
    int level = 0;

    glTexImage2D(GL_TEXTURE_2D, level, format, tex->width, tex->height,
		 0, internal, GL_UNSIGNED_BYTE, tex->data);
    while (tex->width > 1 || tex->height > 1) {
      newtex = mipmap_png_texture(tex, 1, 0, 0);
      freeTextureData(tex);
      tex = newtex;
      level++;
      /*
      fprintf(stderr, "creating mipmap level %d, size(%d, %d)\n", 
	      level, tex->width, tex->height);
      */
      glTexImage2D(GL_TEXTURE_2D, level, format, tex->width, tex->height,
		   0, internal, GL_UNSIGNED_BYTE, tex->data);
    }
  } else { 
      glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0,
		   internal, GL_UNSIGNED_BYTE, tex->data);
  }
    /*
  if(getSettingi("use_mipmaps")) {
    gluBuild2DMipmaps(GL_TEXTURE_2D, format, tex->width, tex->height, 
		      internal, GL_UNSIGNED_BYTE, tex->data);
  } else { 
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0,
		 internal, GL_UNSIGNED_BYTE, tex->data);
  }
    */
  freeTextureData(tex);
}
