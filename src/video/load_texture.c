#include "video/video.h"
#include "filesystem/path.h"

#include "Nebu_scripting.h"

void freeTextureData(texture *tex) {
  free(tex->data);
  free(tex);
}

texture* loadTextureData(const char *filename) {
  texture *tex = NULL;
  char *path;
  char *artpack;
  
	scripting_GetGlobal("settings", "current_artpack", NULL);
  scripting_GetStringResult(&artpack);
  path = getArtPath(artpack, filename);
  free(artpack);
  if(path != NULL) {
    tex = LOAD_TEX(path);
    free(path);
  }

  if(tex == NULL) {    
    fprintf(stderr, "fatal: failed load %s, exiting...\n", filename);
    exit(1); /* OK: critical, installation corrupt */
  }
  return tex;
}

void loadTexture(const char *filename, int format) {
  texture *tex;
  GLint internal;
	int maxSize;
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
	
  tex = loadTextureData(filename);
  if(tex->channels == 3) internal = GL_RGB;
  else internal = GL_RGBA;
  if(format == GL_DONT_CARE) {
    if(tex->channels == 3) format = GL_RGB;
    if(tex->channels == 4) format = GL_RGBA;
  }
  /* TODO: build mipmaps the proper way, box filters suck */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	{
		texture *newtex;
		int level = 0;
    while (tex->width > 1 || tex->height > 1) {
			if(tex->width <= maxSize && tex->height <= maxSize) {
					glTexImage2D(GL_TEXTURE_2D, level, format, 
											 tex->width, tex->height,
											 0, internal, GL_UNSIGNED_BYTE, tex->data);
#ifdef PRINTF_VERBOSE
					printf("uploading level %d, %dx%d texture\n", 
						 level, tex->width, tex->height);
#endif
					level++;
			}
      newtex = mipmap_png_texture(tex, 1, 0, 0);
      freeTextureData(tex);
      tex = newtex;
		}
		/* upload 1x1 mip level */
		glTexImage2D(GL_TEXTURE_2D, level, format, 
								 tex->width, tex->height,
								 0, internal, GL_UNSIGNED_BYTE, tex->data);
#ifdef PRINTF_VERBOSE
		printf("uploading level %d, %dx%d texture\n", 
					 level, tex->width, tex->height);
#endif
		freeTextureData(tex);
	}
}
