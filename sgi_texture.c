#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "sgi_texture.h"

#define ERR_PREFIX "[load_sgi_texture] "
/* must be larger than 512 */
#define BUFSIZE 8192

/* todo: check for EOF errors */

sgi_texture* load_sgi_texture(char *filename) {
  FILE *f;
  unsigned char buf[BUFSIZE];
  unsigned int x, y, bpc, zsize;
  long count, bytes;
  unsigned char *tmp;
  int i, j;
  sgi_texture *tex;
  
  f = fopen(filename, "r");
  if(f == 0) {
    fprintf(stderr, ERR_PREFIX "can't open file %s\n", filename);
    return 0;
  }
  
  fread(buf, 512, 1, f);
  if((buf[0] << 8) + (buf[1] << 0) != 474) {
    fprintf(stderr, ERR_PREFIX "wrong magic: %d %d\n",
	    buf[0], buf[1]);
    return 0;
  }
  
  if(buf[2] != 0) {
    fprintf(stderr, ERR_PREFIX "RLE compression not supported\n");
    return 0;
  }

  if(buf[3] != 1) {
    fprintf(stderr, ERR_PREFIX "BPC is %d - not supported\n", buf[3]);
    return 0;
  }

  bpc = buf[3];  

  zsize = (buf[10] << 8) + buf[11];
  if(zsize != 3 && zsize != 4) {
    fprintf(stderr, ERR_PREFIX "number of channels is %d - not supported\n",
	    zsize);
    return 0;
  }

  x = (buf[6] << 8) + buf[7];
  y = (buf[8] << 8) + buf[9];

  tex = (sgi_texture*) malloc(sizeof(sgi_texture));
  tex->data = malloc(x * y * zsize * bpc);
  tex->width = x;
  tex->height = y;
  tex->channels = zsize;

  count = x * y * zsize * bpc;
  /* fprintf(stderr, ERR_PREFIX "loading %ld bytes\n", count); */
  tmp = (unsigned char*) malloc(count);
  bytes = fread(tmp, count, 1, f);
  /* now, data is in the wrong order: fix that */
  for(i = 0; i < x * y; i++) {
    for(j = 0; j < zsize; j++) {
      *(tex->data + i * zsize + j) = *(tmp + j * x * y + i);
      /* printf("%d -> %d\n", j * x * y + i, i * zsize + j); */
    }
  }
  free(tmp);
  fclose(f);
  return tex;
}

void unload_sgi_texture(sgi_texture *tex) {
  free(tex->data);
  free(tex);
}
