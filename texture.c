#include "gltron.h"

void initTexture(gDisplay *d) {
  GLint min_filter;
  char texname[120];
  int i;
  char fallback[] = "default";
  char *path;

  if(game->settings->use_mipmaps) {
    if(game->settings->mipmap_filter == TRILINEAR)
      min_filter = GL_LINEAR_MIPMAP_LINEAR;
    else
      min_filter = GL_LINEAR_MIPMAP_NEAREST;
  } else
    min_filter = GL_LINEAR;

  checkGLError("texture.c initTexture - start");
  /* todo: move that somewhere else */
  glGenTextures(game_textures, d->textures);
  checkGLError("texture.c initTexture - creating textures");
  for(i = 0; i < n_textures; i++) {
    glBindTexture(GL_TEXTURE_2D, d->textures[ textures[i].id ]);
    /* todo: snprintf would be safer, but win32 doesn't have it */
    sprintf(texname, "%s%c%s%s", 
	     d->artpack.path, SEPERATOR, textures[i].name, TEX_SUFFIX);
    if((path = getFullPath(texname)) == NULL) {
      sprintf(texname, "%s%c%s%s", 
	       fallback, SEPERATOR, textures[i].name, TEX_SUFFIX);
    } else {
      free(path);
    }
    loadTexture(texname, textures[i].type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textures[i].wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textures[i].wrap_t);
    checkGLError("texture.c initTextures");
  }
}

void deleteTextures(gDisplay *d) {
  glDeleteTextures(game_textures, d->textures);
  ogl_debugtex = 0;

  checkGLError("texture.c deleted textures");
}
