#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

typedef struct {
  int mouse1;
  int mouse2;
  int mousex;
  int mousey;
} Input;

typedef struct {
  int win_id;     /* only the root visual has an id */
  int h, w;       /* window dimension */
  int vp_x, vp_y; /* viewport position */
  int vp_h, vp_w; /* viewport dimension */

  int onScreen;

  unsigned int *textures; /* texture IDs */
} Visual;

enum { VP_SINGLE = 0, VP_SPLIT = 1, VP_FOURWAY = 2 }; // Viewport Type;

#endif
