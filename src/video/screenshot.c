#include "video/video.h"
#include "filesystem/path.h"
#include "Nebu_filesystem.h"

#include <png.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

#define SCREENSHOT_PREFIX "gltron"
#define SCREENSHOT_PNG_BITDEPTH 8
#define SCREENSHOT_BYTES_PER_PIXEL 3

typedef struct {
  int width;
  int height;
  unsigned char *pixmap;
} screenshot_info_t;

static FILE *fp;

static void user_write_data(png_structp png_ptr,
		     png_bytep data, png_size_t length) {
  fwrite(data, length, 1, fp);
}

static void user_flush_data(png_structp png_ptr) {
  fflush(fp);
}

static int writePixmapToPng(screenshot_info_t *screenshot, char *fname) {
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte **row_ptrs;
  int i;

  if (!(fp = fopen(fname, "wb"))) {
    fprintf(stderr, "can't open %s for writing\n", fname);
    return -1;
  }

  if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
                                          NULL, NULL, NULL))) {
    return -1;
  }

  if (!(info_ptr = png_create_info_struct(png_ptr))) {
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    return -1;
  }

  png_set_write_fn(png_ptr, 0, user_write_data, user_flush_data);
  /* png_init_io(png_ptr, fp); */

  png_set_IHDR(png_ptr, info_ptr, screenshot->width, screenshot->height,
	             SCREENSHOT_PNG_BITDEPTH, PNG_COLOR_TYPE_RGB, 
	             PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
	             PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);

  /* get pointers */
  if(!(row_ptrs = (png_byte**) malloc(screenshot->height * sizeof(png_byte*)))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return -1;
  }
  
  for(i = 0; i < screenshot->height; i++) {
    row_ptrs[i] = screenshot->pixmap + (screenshot->height - i - 1) 
      * SCREENSHOT_BYTES_PER_PIXEL * screenshot->width;
  }

  png_write_image(png_ptr, row_ptrs);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr); 
  
  free(row_ptrs);
  return 0;
}

/*
    getNextFilename - find the next free filename in series.
 */
static char* getNextFilename(const char *suffix, int *start_at) {
  char *path = NULL;
  char fname[PATH_MAX];
  do {
    if(path != NULL)
      free(path);

    (*start_at)++;
    sprintf(fname, "%s-%s-%d%s", SCREENSHOT_PREFIX, VERSION, *start_at, 
            suffix);
    path = getPossiblePath( PATH_SNAPSHOTS, fname );
  } while ( fileExists(path));

  return path;
}
  
static int captureScreenToPixmap(screenshot_info_t *img, Visual *display) { 
  img->width = display->w;
  img->height = display->h;
  if (!(img->pixmap = malloc(img->width * 
                             img->height * SCREENSHOT_BYTES_PER_PIXEL))) {
    return -1;
  }
  glReadPixels(0, 0, img->width, img->height, GL_RGB, GL_UNSIGNED_BYTE, 
               img->pixmap);
  return 0;
}

void doPngScreenShot(Visual *display) {
  screenshot_info_t screenshot;
  static int last_png_num; /* store last free file index. */
  char *path;
  path = getNextFilename(".png", &last_png_num);
  if(path != NULL) {
    if (captureScreenToPixmap(&screenshot, display) != 0) {
      fprintf(stderr, "Error capturing screenshot\n");
      return;
    }
  
    if (writePixmapToPng(&screenshot, path) != 0) {
      fprintf(stderr, "Error writing screenshot %s\n", path);
    } else {
      fprintf(stderr, "Screenshot written to %s\n", path);
    }
    free(screenshot.pixmap);
    free(path);
  }
}

void doBmpScreenShot(Visual *display) {
  screenshot_info_t screenshot;
  static int last_bmp_num; /* store last free file index. */
  char *path;
  path = getNextFilename(".bmp", &last_bmp_num);
  if(path != NULL) {
    if (captureScreenToPixmap(&screenshot, display) != 0) {
      fprintf(stderr, "Error capturing screenshot\n");
      return;
    }
  
    if (SystemWriteBMP(path, screenshot.width, screenshot.height,
		       screenshot.pixmap) != 0) {
      fprintf(stderr, "Error writing screenshot %s\n", path);
    } else {
      fprintf(stderr, "Screenshot written to %s\n", path);
    }
  
    free(screenshot.pixmap);
    free(path);
  }
}

