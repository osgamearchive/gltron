#include "gltron.h"

void initTexture(gDisplay *d) {
  GLint min_filter;
  char texname[120];

  int i, j;

  if(getSettingi("use_mipmaps")) {
    if(getSettingi("mipmap_filter") == TRILINEAR)
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
    for( j = 0; j < textures[i].count; j++) {
      glBindTexture(GL_TEXTURE_2D, d->textures[ textures[i].id + j ]);
      /* todo: snprintf would be safer, but win32 doesn't have it */
      if(textures[i].count == 1) {
	sprintf(texname, "%s%s", textures[i].name, TEX_SUFFIX);
      } else {
	sprintf(texname, "%s%d%s", textures[i].name, j, TEX_SUFFIX);
      }
      loadTexture(texname, textures[i].type);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textures[i].wrap_s);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textures[i].wrap_t);

      if(getSettingi("softwareRendering")) {

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      } else {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

	if(renderer.ext_filter_anisotropic) {
	  /* fprintf(stderr, "enabling anisotropic filtering\n"); */
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
			  textures[i].anisotropy);
	}
      }
      checkGLError("texture.c initTextures");
    }
  }
}

void deleteTextures(gDisplay *d) {
  glDeleteTextures(game_textures, d->textures);
  ogl_debugtex = 0;

  checkGLError("texture.c deleted textures");
}




