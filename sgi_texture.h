#ifndef SGI_TEXTURE_H
#define SGI_TEXTURE_H

typedef struct {
  int width;
  int height;
  int channels;
  unsigned char *data;
} sgi_texture;

extern sgi_texture* load_sgi_texture(char *filename);
extern void unload_sgi_texture(sgi_texture *tex);

#endif
