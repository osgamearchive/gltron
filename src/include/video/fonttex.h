#ifndef FONTTEX_H
#define FONTTEX_H

typedef struct {
  int nTextures;
  
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  int *texID;

  char *fontname;
  char *bitmapname;
  int bitmapTexwidth;
  int bitmapWidth; /* character width */
} FontTex;

#endif
