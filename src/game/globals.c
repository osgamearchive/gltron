#include "game/game.h"
#include "video/video.h"
#include "input/input.h"

/* globals */
Game main_game;
Game *game;
Game2 main_game2;
Game2 *game2;
Visual *gScreen;
int gViewportType;
PlayerVisual *gPlayerVisuals;
Input gInput;

SettingsCache gSettingsCache;

int viewport_content[4]; /* max. 4 individual viewports on the screen */

/* TexFont *txf = NULL; */
FontTex *guiFtx = NULL;
FontTex *gameFtx = NULL;
int fontID = 6;

Mesh* recognizer_quad;
float rec_outline_color[] = {0.8f, 0.0, 0.0};
float rec_spec_color[] = {0.05f, 0.14f, 0.05f, 0.50f};

Mesh* recognizer;
Mesh* lightcycle[LC_LOD];
char *lc_lod_names[] = {
  "lightcycle-high.obj",
  "lightcycle-med.obj",
  "lightcycle-low.obj"
};

float camAngle = 0;
float cam_phi = 0;
float cam_chi = PI / 6.0f;
float cam_r_mouse = CAM_CIRCLE_DIST;
float cam_r_follow = CAM_FOLLOW_DIST;
float cam_r_circle = CAM_CIRCLE_DIST;

unsigned char* debugtex = NULL;
int ogl_debugtex;
int colwidth;
segment2 walls[4];

int dirsX[] = { 0, -1, 0, 1 };
int dirsY[] = { -1, 0, 1, 0 };
float camAngles[] = { PI / 2, 0, 3 * PI / 2, PI, 2 * PI };

float cam_defaults[][3] =  { 
  { CAM_CIRCLE_DIST, PI / 3, 0 }, /* circle */
  { CAM_FOLLOW_DIST, PI / 4, PI / 72 }, /* follow */
  { CAM_COCKPIT_Z, PI / 8, 0 }, /* cockpit */
  { CAM_CIRCLE_DIST, PI / 3, 0 } /* free */
};

/* int lasttime; 
   double dt; */
/* milliseconds since last frame */

int polycount; /* poly count - how much do we draw each frame? */

unsigned char debugcolors[6][4] = {
  { 0, 0, 0, 0 },
  { 255, 0, 0, 255 },
  { 0, 255, 255, 255 },
  { 0, 255, 0, 255 },
  { 0, 0, 255, 255 },
  { 255, 255, 255, 255 }
};

int vps = 4;
int vp_max[] = { 1, 2, 4, 1 }; /* windows in this display setting */

/* currently, the screen is partitioned into 32 units horizontally,
   and 24 units vertically */

/* these arrays have four members, for the four different visuals:
   single, split, quad, unused */

/* the offset of the view, in virtual units */
float vp_x[4][4] = { { 0 },    { 1, 1 },  { 1, 16, 1, 16 }, { 10 } };
float vp_y[4][4] = { { 0 },    { 0.5f, 12.5f },   { 1, 1, 12.5f, 12.5f }, { 4.5 } } ;

/* the width/height of the view, in virtual units */
float vp_w[4][4] = { { 32 },   { 30, 30 }, { 14, 14, 14, 14 }, { 20 } };
float vp_h[4][4] = { { 24 }, { 11.5f, 11.5f }, { 10.5f, 10.5f, 10.5f, 10.5f }, { 15 } };

#ifdef DEPTH_SORT
int quadBufSize = 100;
Quad *quadBuf;
int *quadBufIndex;
#endif

int game_textures = TEX_COUNT + 5; /* 11 + 6 + debug texture */
int n_textures = 11;

#ifndef GL_VERSION_1_2
#  define GL_CLAMP_TO_EDGE GL_CLAMP
#endif

TextureInfo textures[] = {
  { 1, "gltron_floor", GL_DONT_CARE, GL_REPEAT, GL_REPEAT, TEX_FLOOR, 64 },
  { 1, "gltron", GL_RGB, GL_REPEAT, GL_REPEAT, TEX_GUI, 1 },
  { 1, "gltron_logo", GL_RGBA, 
    GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, TEX_LOGO, 1 },
  { 1, "gltron_wall_1", GL_DONT_CARE, 
    GL_REPEAT, GL_CLAMP_TO_EDGE, TEX_WALL1, 1 },
  { 1, "gltron_wall_2", GL_DONT_CARE, 
    GL_REPEAT, GL_CLAMP_TO_EDGE, TEX_WALL2, 1 },
  { 1, "gltron_wall_3", GL_DONT_CARE, 
    GL_REPEAT, GL_CLAMP_TO_EDGE, TEX_WALL3, 1 },
  { 1, "gltron_wall_4", GL_DONT_CARE, 
    GL_REPEAT, GL_CLAMP_TO_EDGE, TEX_WALL4, 1 },
  { 1, "gltron_trail", GL_RGBA, 
    GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, TEX_TRAIL, 1 },
  { 1, "gltron_traildecal", GL_RGBA, 
    GL_REPEAT, GL_CLAMP_TO_EDGE, TEX_DECAL, 1 },
  { 6, "skybox", GL_RGB, 
    GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, TEX_SKYBOX, 1 },
  { 1, "gltron_impact", GL_RGBA, GL_REPEAT, GL_CLAMP_TO_EDGE, TEX_IMPACT, 1 },

};

float shadow_color[] = { 0, 0, 0, .8f };

#define LX 2.0f
#define LY 2.0f
float shadow_matrix[] = { LX * LY, 0,       0, 0, 
			  0,       LX * LY, 0, 0, 
			  -LY,    -LX,      0, 0, 
			  0,       0,       0, LX * LY };
#undef LX
#undef LY


