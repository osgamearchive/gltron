#include "video/video.h"

void initTexture(Visual *d) {
  GLint min_filter;
  char texname[120];

  int i, j;

  switch(getSettingi("mipmap_filter")) {
	case NEAREST:
		min_filter = GL_NEAREST;
		break;
	case LINEAR:
		min_filter = GL_LINEAR;
		break;
	case MIPMAP:
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
		break;
	case TRILINEAR:
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
		break;
	default:
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
	}
	printf("using min_filter: %d (setting: %d)\n", min_filter,
				 getSettingi("mipmap_filter"));

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

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#endif

#if 0
				if(renderer.ext_filter_anisotropic) {
					/* fprintf(stderr, "enabling anisotropic filtering\n"); */
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
													textures[i].anisotropy);
				}
#endif
      }
      checkGLError("texture.c initTextures");
    }
  }
}

void deleteTextures(Visual *d) {
  glDeleteTextures(game_textures, d->textures);
  checkGLError("texture.c deleted textures");
}




