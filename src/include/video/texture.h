#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  int count;
  char name[48];
  int type;
  int wrap_s;
  int wrap_t;
  int id;
  float anisotropy;
} TextureInfo;

enum {
  TEX_FLOOR = 0,
  TEX_GUI,
  TEX_LOGO,
  TEX_WALL1,
  TEX_WALL2,
  TEX_WALL3,
  TEX_WALL4,
  TEX_IMPACT,
  TEX_TRAIL,
  TEX_DECAL,
  TEX_DEBUG,
  TEX_SKYBOX,
	/* insert all texture tokens before TEX_COUNT */
	TEX_COUNT
};

#endif

