#ifndef FONTTEX_H
#define FONTTEX_H

#include <stdio.h>
#include <stdlib.h>
#include "sgi_texture.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

typedef struct fontbmp {
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  char *bitmapname;
} fontbmp;

typedef struct fonttex {
  // sgi_texture **textures;
  int nTextures;
  
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  unsigned int *texID;

  char *fontname;
  char *bitmapname;
  int bitmapTexwidth;
  int bitmapWidth; /* character width */
} fonttex;

extern fonttex *ftxLoadFont(char *filename);
extern void ftxUnloadFont(fonttex *ftx);
extern void loadTexture(char *filename, int format);

/* It's not necessary to bind the texture explicitly. */
/* (and we don't know which texture to bind) */

/* ftxRenderString will take care of that */
/* extern void ftxBindFontTexture(fontTex *ftx); */

extern void ftxRenderString(fonttex *ftx, char *string, int len);

/* extern void ftxGetStringWidth(fontTex *ftx, */
/*                               char *string, int len, int *width); */
/* can't get max_ascent, max_descent yet */

#endif







