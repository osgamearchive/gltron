#include "gltron.h"

#define SGI_TEX
/* #define SDL_TEX */

#ifdef SGI_TEX
#include "sgi_texture.h"

void loadTexture(char *filename, int format) {
  char *path;
  sgi_texture *tex;

  path = getFullPath(filename);
  if(path != 0)
    tex = load_sgi_texture(path);
  else {
    fprintf(stderr, "fatal: could not load %s, exiting...\n", filename);
    exit(1);
  }
  /* TODO: build mipmaps the proper way, box filters suck */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if(game->settings->use_mipmaps) {
    gluBuild2DMipmaps(GL_TEXTURE_2D, format, tex->width, tex->height, 
		      GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
  } else { 
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
  }

  free(tex->data);
  free(tex);
}
#endif

#ifdef SDL_TEX

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


int loadTexture(char *name, int wantedFormat) {
  /* wantedFormat is ignored */
  SDL_Surface *image;
  int tex_id;
  int format;
  int x, y;
  int bpp;
  unsigned char *data;

  image = IMG_Load(name);

  if(image == 0) {
    fprintf(stderr, "can't load image: %s\n", SDL_GetError());
    exit(1);
  }

  /* now make the thing a usable OpenGL texture */

  /* support 24 & 32 bit images */
  bpp = image->format->BitsPerPixel;
  if(bpp != 24 && bpp != 32) {
    SDL_FreeSurface(image);
    return -1;
  }
  
  printf("bpp: %d\n", bpp);
  data = (unsigned char*) malloc(image->w * image->h * bpp / 8);
  /* copy every pixel */
  for(y = 0; y < image->h; y++) {
    for(x = 0; x < image->h; x++) {

      Uint8 *bufp;
      bufp = (Uint8 *)image->pixels + y*image->pitch + (bpp / 8) * x;
      *(data + (bpp / 8) * (y * image->w + x) + 0) =
	*(bufp + image->format->Rshift / 8);
      *(data + (bpp / 8) * (y * image->w + x) + 1) =
	*(bufp + image->format->Gshift / 8);
      *(data + (bpp / 8) * (y * image->w + x) + 2) =
	*(bufp + image->format->Bshift / 8);
      /* why on earth are the alpha values reversed? */
      if(bpp == 32) {
	*(data + (bpp / 8) * (y * image->w + x) + 3) =
	  255 - *(bufp + image->format->Ashift / 8);
      }
    }
  }

  format = (bpp == 24) ? GL_RGB : GL_RGBA;

  /* load the texture into OGL */
  /* TODO: build mipmaps the proper way, box filters suck */

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  /*   glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0,
       format, GL_UNSIGNED_BYTE, data); */
  gluBuild2DMipmaps(GL_TEXTURE_2D, format, image->w, image->h, 
                    format, GL_UNSIGNED_BYTE, data);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  free(data);
  SDL_FreeSurface(image);
  return 0;
}

#endif

