#include "gltron.h"

/* globals */
Game main_game;
Game *game;
Game2 main_game2;
Game2 *game2;

char messages[80];

/* TexFont *txf = NULL; */
fonttex *guiFtx = NULL;
fonttex *gameFtx = NULL;
int fontID = 6;

Menu** pMenuList;
Menu* pRootMenu;

int *configureKeyEntry;
Menu *configureKeyMenu;

float camAngle = 0;
float cam_phi = 0;
float cam_chi = 3.14 / 6.0;
float cam_r = CAM_CIRCLE_DIST;

unsigned char* colmap = NULL;
unsigned char* debugtex = NULL;
int ogl_debugtex;
int colwidth;

int dirsX[] = { 0, -1, 0, 1 };
int dirsY[] = { -1, 0, 1, 0 };

// screensaver hack
int stoptime = 0;
// menu hack
int menutime;
/* int lasttime; 
   double dt; */
/* milliseconds since last frame */

/* settings stuff */

settings_int *si;
int si_count;
settings_float *sf;
int sf_count;


int polycount; /* poly count - how much do we draw each frame? */
int screenshots = 0;

/* default settings */

// int default_arena_sizes[] = { 160, 240, 400, 600, 1200 };
int default_arena_sizes[] = { 160, 240, 480, 720, 1200 };
// int default_texture_modifier[] = { be 
float default_speeds[] = { 5.0, 6.5, 8.5, 12.0, 0.0 };
/* float colors_alpha[][4] = { { 1.0, 0.2, 0.4 , 0.4}, { 0.2, 0.3, 1.0, 0.4}, */
float colors_alpha[][4] = { { .6, 0.6, 0.0, 0.6}, { 0.5, 0.05, 0.25, 0.6},
		       { 0.2, 0.6, 0.4, 0.6 }, { 0.7, 0.7, 0.7, 0.6 } };

float colors_model[][4] = { { 1.0, 1.0, 0.0, 1.0 }, { 0.7, 0.1, 0.1, 1.0 },
		       { 0.3, 1.0, 0.8, 1.0 }, { 0.8, 0.8, 0.8, 1.0 } };

unsigned char debugcolors[6][4] = {
  { 0, 0, 0, 0 },
  { 255, 0, 0, 255 },
  { 0, 255, 255, 255 },
  { 0, 255, 0, 255 },
  { 0, 0, 255, 255 },
  { 255, 255, 255, 255 }
};

float menu_fgColor[4] = { 1.0, 1.0, 1.0, 1.0 };
float menu_hlColor1[4] = { 1.0, 0.2, 0.2, 1.0 };
float menu_hlColor2[4] = { 1.0, 1.0, 0.0, 1.0 };

int vps = 4;
int vp_max[] = { 1, 2, 4, 1 }; /* windows in this display setting */
float vp_x[4][4] = { { 1 },    { 1, 1 },  { 1, 16, 1, 16 }, { 10 } };
float vp_y[4][4] = { { 1 },    { 0.5, 12.5 },   { 1, 1, 12.5, 12.5 }, { 4.5 } } ;
float vp_w[4][4] = { { 30 },   { 30, 30 }, { 14, 14, 14, 14 }, { 20 } };
float vp_h[4][4] = { { 22.5 }, { 11.5, 11.5 }, { 10.5, 10.5, 10.5, 10.5 }, { 15 } };

#ifdef DEPTH_SORT
int quadBufSize = 100;
Quad *quadBuf;
int *quadBufIndex;
#endif


int game_textures = 11; /* 10 + debug texture */
int n_textures = 10;

texture_info textures[] = {
  { "gltron_floor", GL_RGB, GL_REPEAT, GL_REPEAT, TEX_FLOOR },
  { "gltron", GL_RGB, GL_REPEAT, GL_REPEAT, TEX_GUI },
  { "gltron_logo", GL_RGBA, GL_CLAMP, GL_CLAMP, TEX_LOGO },
  { "gltron_wall_1", GL_RGB, GL_REPEAT, GL_CLAMP, TEX_WALL1 },
  { "gltron_wall_2", GL_RGB, GL_REPEAT, GL_CLAMP, TEX_WALL2 },
  { "gltron_wall_3", GL_RGB, GL_REPEAT, GL_CLAMP, TEX_WALL3 },
  { "gltron_wall_4", GL_RGB, GL_REPEAT, GL_CLAMP, TEX_WALL4 },
  { "gltron_crash", GL_RGBA, GL_REPEAT, GL_CLAMP, TEX_CRASH },
  { "gltron_trail", GL_RGBA, GL_CLAMP, GL_CLAMP, TEX_TRAIL },
  { "gltron_traildecal", GL_RGBA, GL_REPEAT, GL_CLAMP, TEX_DECAL }
};












