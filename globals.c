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

Mesh* recognizer;

Menu** pMenuList;
Menu* pRootMenu;

int *configureKeyEntry;
Menu *configureKeyMenu;

float camAngle = 0;
float cam_phi = 0;
float cam_chi = 3.14 / 6.0;
float cam_r_mouse = CAM_CIRCLE_DIST;
float cam_r_follow = CAM_FOLLOW_DIST;
float cam_r_circle = CAM_CIRCLE_DIST;

unsigned char* colmap = NULL;
unsigned char* debugtex = NULL;
int ogl_debugtex;
int colwidth;

int dirsX[] = { 0, -1, 0, 1 };
int dirsY[] = { -1, 0, 1, 0 };
float camAngles[] = { M_PI / 2, 0, 3 * M_PI / 2, M_PI, 2 * M_PI };

char *cam_names[] = { "circle", "follow", "cockpit", "free" };
float cam_defaults[][3] =  { 
  { CAM_CIRCLE_DIST, M_PI / 3, 0 }, /* circle */
  { CAM_FOLLOW_DIST, M_PI / 4, M_PI / 72 }, /* follow */
  { CAM_COCKPIT_Z, 0, 0 }, /* cockpit */
  { CAM_CIRCLE_DIST, M_PI / 3, 0 } /* free */
};

// screensaver hack
int stoptime = 0;
// menu hack
int menutime;
/* int lasttime; 
   double dt; */
/* milliseconds since last frame */

/* Networks stuff */
#ifdef __NETWORK__
Slots  slots[4]; //slots of the server.

int    nbUsers = 0; 
int    serverstate = preGameState;
char   name[255];
int    sChat       = 0;
char   mesg[255] = "";
int    me;

netEventList neteventlist = NULL;

int    packet_type = HEADER;
Netscores         netscores;
#endif

/* settings stuff */

settings_int *si;
int si_count;
settings_float *sf;
int sf_count;
settings_v *sv;
int sv_count;


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


int game_textures = 17; /* 10 + 6 + debug texture */
int n_textures = 11;

texture_info textures[] = {
  { 1, "gltron_floor", GL_DONT_CARE, GL_REPEAT, GL_REPEAT, TEX_FLOOR },
  { 1, "gltron", GL_RGB, GL_REPEAT, GL_REPEAT, TEX_GUI },
  { 1, "gltron_logo", GL_RGBA, GL_CLAMP, GL_CLAMP, TEX_LOGO },
  { 1, "gltron_wall_1", GL_DONT_CARE, GL_REPEAT, GL_CLAMP, TEX_WALL1 },
  { 1, "gltron_wall_2", GL_DONT_CARE, GL_REPEAT, GL_CLAMP, TEX_WALL2 },
  { 1, "gltron_wall_3", GL_DONT_CARE, GL_REPEAT, GL_CLAMP, TEX_WALL3 },
  { 1, "gltron_wall_4", GL_DONT_CARE, GL_REPEAT, GL_CLAMP, TEX_WALL4 },
  { 1, "gltron_crash", GL_RGBA, GL_REPEAT, GL_CLAMP, TEX_CRASH },
  { 1, "gltron_trail", GL_RGBA, GL_CLAMP, GL_CLAMP, TEX_TRAIL },
  { 1, "gltron_traildecal", GL_RGBA, GL_REPEAT, GL_CLAMP, TEX_DECAL },
  { 6, "skybox", GL_RGB, GL_CLAMP, GL_CLAMP, TEX_SKYBOX }

};

float shadow_color[] = { 0, 0, 0, .8 };
// float shadow_color[] = { 1, 1, 1, 1 };
#define LX 2.0
#define LY 2.0
float shadow_matrix[] = { LX * LY, 0,       0, 0, 
			  0,       LX * LY, 0, 0, 
			  -LY,    -LX,      0, 0, 
			  0,       0,       0, LX * LY };
#undef LX
#undef LY
