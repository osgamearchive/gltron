#ifndef PNG_TEXTURE_H
#define PNG_TEXTURE_H

typedef struct {
  int width;
  int height;
  int channels;
  unsigned char *data;
} png_texture;

extern png_texture* load_png_texture(char *filename);
extern void unload_png_texture(png_texture *tex);

#endif

