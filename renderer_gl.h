#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <stdio.h>
#include "quad.h"

#define POLY_COUNT

extern void printRendererInfo();
extern void renderQuad(Quad *q);
extern void clearState();

typedef struct GLstate {
  int tex_id;
  int tex_env_mode;
  int binds;
  int mod_changes;
} GLstate;

extern GLstate *state;
