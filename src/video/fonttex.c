#include "video/video.h"
#include "video/nebu_renderer_gl.h"
#include "filesystem/path.h"
#include "Nebu_filesystem.h"

#include <stdio.h>
#include <stdlib.h>


#define FTX_ERR "[FontTex error]: "

FontTex *ftxLoadFont(const char *filename) {
  char *path;
  FontTex *ftx;
  
  path = getPath(PATH_DATA, filename);
  if(path == NULL) {
    fprintf(stderr, FTX_ERR "can't load font file '%s'\n", filename);
    return NULL;
  }
  ftx = nebu_Font_Load(path);
  free(path);
  return ftx;
}

void ftxUnloadFont(FontTex *ftx) {
	nebu_Font_Free(ftx);
}

static int color_base = 48;
static int colors = 8;
enum {
  white = 0,
  black,
  red,
  orange,
  yellow,
  lt_green,
  green,
  blue_green
};

unsigned char color_codes[][3] = {
  { 255, 255,255 },
  { 0, 0, 0 },
  { 255, 0, 0 },
  { 255, 128, 0 },
  { 255, 255, 0 },
  { 128, 255, 0 },
  { 0, 255, 0 },
  { 0, 255, 128 }
};

/*
0     48    white       65535  65536  65535  FFFFFF
1     49    black           0      0      0  000000

2     50    red         65535      0      0  FF0000
3     51    orange      65535  32768      0  FF8000
4     52    yellow      65535  65535      0  FFFF00
5     53    lt green    32768  65535      0  80FF00
6     54    green           0  65535      0  00FF00
7     55    blue green      0  65535  32768  00FF80
8     56    cyan            0  65535  65535  00FFFF
9     57    lt blue         0  32768  65535  0080FF
:     58    blue            0      0  65535  0000FF
;     59    purple      32768      0  65535  8000FF
< 60    magenta     65535      0  65535  FF00FF
=     61    purple red  65535      0  32768  FF0080

> 62    lt gray     49152  49152  49152  C0C0C0
?     63    dk gray     16384  16384  16384  404040

@     64    -           32768      0      0  800000
A     65    |           32768  16384      0  804000
B     66    |           32768  32768      0  808000
C     67    | darker    16384  32768      0  408000
D     68    | versions      0  32768      0  008000
E     69    | of            0  32768  16384  008040
F     70    | colors        0  32768  32768  008080
G     71    | 50..61        0  16384  32768  004080
H     72    |               0      0  32768  000080
I     73    |           16384      0  32768  400080
J     74    |           32768      0  32768  800080
K     75    -           32768      0  16384  800040
*/

void ftxRenderString(FontTex *ftx, const char *string, int len) {
  int i;
  int bound = -1;
  int index;
  
  int tex;
  int w;
  float cw;
  float cx, cy;

  w = ftx->texwidth / ftx->width;
  cw = (float)ftx->width / (float)ftx->texwidth;

  for(i = 0; i < len; i++) {
    if(string[i] == 3) { /* color code */
      i++;
      if(i >= len) return;
      if(string[i] < color_base && string[i] > color_base + colors) continue;
      glColor3ubv(color_codes[ string[i] - color_base ]);
      continue;
    }
      
    /* find out which texture it's in */
    /* TODO(4): find out why the +1 is necessary */
    index = string[i] - ftx->lower + 1;
    if(index >= ftx->upper) 
      fprintf(stderr, FTX_ERR " index out of bounds");
    tex = index / (w * 
w);
    /* bind texture */
    if(tex != bound) {
      glBindTexture(GL_TEXTURE_2D, ftx->texID[tex]);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      bound = tex;
    }
    /* find texture coordinates */
    index = index % (w * w);
    cx = (float)(index % w) / (float)w;
    cy = (float)(index / w) / (float)w;
    /* draw quad */
    /* fprintf(stderr, FTX_ERR "coords: tex %d (%.2f, %.2f), %.2f\n", */
    /*     bound, cx, cy, cw); */

    glBegin(GL_QUADS);
    glTexCoord2f(cx, 1 - cy - cw);
    glVertex2i(i, 0);
    glTexCoord2f(cx + cw, 1 - cy - cw);
    glVertex2i(i + 1, 0);
    glTexCoord2f(cx + cw, 1 - cy);
    glVertex2i(i + 1, 1);
    glTexCoord2f(cx, 1 - cy);
    glVertex2i(i, 1);
    glEnd();
  }
  /* nebu_Video_CheckErrors("FontTex.c ftxRenderString\n"); */
}
