#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  int count;
  char name[48];
  int type;
  int wrap_s;
  int wrap_t;
  int id;
} texture_info;

enum {
  TEX_FLOOR = 0,
  TEX_GUI,
  TEX_LOGO,
  TEX_WALL1,
  TEX_WALL2,
  TEX_WALL3,
  TEX_WALL4,
  TEX_CRASH,
  TEX_TRAIL,
  TEX_DECAL,
  TEX_DEBUG,
  TEX_SKYBOX,
#ifdef __NETWORK__
  TEX_PREGAME
#endif
};

#endif

