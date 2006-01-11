#include "video/video.h"
#include "configuration/settings.h"

#include "base/nebu_surface.h"
#include "video/nebu_2d.h"
#include "video/nebu_video_system.h"
#include "video/nebu_renderer_gl.h"

void initTexture(Visual *d) {
  GLint min_filter;

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

  nebu_Video_CheckErrors("texture.c initTexture - start");
  /* todo: move that somewhere else */
  glGenTextures(TEX_COUNT, d->textures);
  nebu_Video_CheckErrors("texture.c initTexture - creating textures");
  for(i = 0; i < TEX_COUNT; i++) {
    for( j = 0; j < textures[i].count; j++) {
      glBindTexture(GL_TEXTURE_2D, d->textures[ textures[i].id + j ]);
      loadTexture(textures[i].name, textures[i].type);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textures[i].wrap_s);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textures[i].wrap_t);

      if(getSettingi("softwareRendering")) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      } else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

				if(GLEW_EXT_texture_filter_anisotropic) {
					/* fprintf(stderr, "enabling anisotropic filtering\n"); */
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
													textures[i].anisotropy);
				}
      }
      nebu_Video_CheckErrors("texture.c initTextures");
    }
  }
}

void deleteTextures(Visual *d) {
  glDeleteTextures(TEX_COUNT, d->textures);
  nebu_Video_CheckErrors("texture.c deleted textures");
}




