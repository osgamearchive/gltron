#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

typedef struct Input {
  int mouse1;
  int mouse2;
  int mousex;
  int mousey;
} Input;

typedef struct fontbmp {
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  texture *tex;
} fontbmp;

typedef struct gDisplay {
  int win_id;     /* nur das globale Window hat eine */
  int h, w;       /* window */
  int vp_x, vp_y; /* viewport */
  int vp_h, vp_w;
  int blending;
  int fog;
  int shademodel;
  int wall;
  int onScreen;

  /* texture ID */
  unsigned int *textures;

  /* software rendering stuff */
  unsigned char *pixelGui;
  fontbmp *bitfont;
} gDisplay;


typedef enum ViewportType {
    VP_SINGLE = 0,
    VP_SPLIT = 1,
    VP_FOURWAY = 2
} ViewportType;

#endif
