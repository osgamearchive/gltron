#include "gltron.h"

#include <png.h>

#include <stdio.h>
#include <stdlib.h>

int screenShot(char *filename, gDisplay *d);

void doScreenShot() {
  char buf[100];
  
  sprintf(buf, "gltron-0.59-%d.png", screenshots);
  screenShot(buf, game->screen);
  screenshots++;
}

FILE *fp;

void user_write_data(png_structp png_ptr,
		     png_bytep data, png_uint_32 length) {
  fwrite(data, length, 1, fp);
}

void user_flush_data(png_structp png_ptr) {
  fflush(fp);
}

int screenShot(char *filename, gDisplay *d) {
  unsigned char *data;

  png_structp png_ptr;
  png_infop info_ptr;
  png_byte **row_pointers;
  int colortype;
  int width, height;
  int i;

  width = d->w;
  height = d->h;
  data = malloc(width * height * 3);
  
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
  
  fp = fopen(filename, "wb");
  if(!fp) {
    fprintf(stderr, "can't open %s for writing\n", filename);
    return 1;
  }
  
  png_ptr = png_create_write_struct
    /*     (PNG_LIBPNG_VER_STRING, (png_voidp)user_error_ptr,
	   user_error_fn, user_warning_fn); */
    (PNG_LIBPNG_VER_STRING, 0, 0, 0);

  if (!png_ptr)
    return 1;

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
      png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
      return 1;
  }
  /* png_init_io(png_ptr, fp); */
  png_set_write_fn(png_ptr, 0, user_write_data, user_flush_data);

  colortype = PNG_COLOR_TYPE_RGB;

  png_set_IHDR(png_ptr, info_ptr, width, height,
	       8, colortype, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);

  /* get pointers */
  row_pointers = (png_byte**) malloc(height * sizeof(png_byte*));
  for(i = 0; i < height; i++)
    row_pointers[i] = data + (height - i - 1) 
      * 3 * width;

  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  
  free(row_pointers);
  free(data);

  fprintf(stderr, "written screenshot to %s\n", filename);
  return 0;
}
  



