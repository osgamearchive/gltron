#ifndef DATA_H
#define DATA_H

#include "basic_types.h"
#include "game_data.h"
#include "client_data.h"

/* general data structures */
  
typedef struct {
  GLint nTextures;
  
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  GLuint *texID;

  char *fontname;
  char *bitmapname;
  int bitmapTexwidth;
  int bitmapWidth; /* character width */
} FontTex;

typedef struct {
  char *path;
  /* char *name; currently ignored */
} Artpack;

#define PLAYER_IS_ACTIVE(x) ((x)->data->speed > 0)

enum {
  GAME_SINGLE = 1,
#ifdef RECORD
  GAME_SINGLE_RECORD = 2,
  GAME_PLAY = 4,
  GAME_PLAY_NETWORK = 8,
  GAME_NETWORK_RECORD
#endif
};

enum {
  BILINEAR = 0,
  TRILINEAR = 1
};

#endif
