#include "gltron.h"

/* globals */
Game main_game;
Game *game;

/* TexFont *txf = NULL; */
fonttex *guiFtx = NULL;
fonttex *gameFtx = NULL;
int fontID = 6;

Menu** pMenuList;
Menu* pRootMenu;

int *configureKeyEntry;
Menu *configureKeyMenu;

float camAngle = 0;

unsigned char* colmap = NULL;
unsigned char* debugtex = NULL;
int ogl_debugtex;
int colwidth;

int dirsX[] = { 0, -1, 0, 1 };
int dirsY[] = { -1, 0, 1, 0 };

int lasttime; 
double dt; /* milliseconds since last frame */

/* settings stuff */

settings_int *si;
int si_count;
settings_float *sf;
int sf_count;


int polycount; /* poly count - how much do we draw each frame? */
int screenshots = 0;

/* default settings */

int default_arena_sizes[] = { 160, 240, 400, 600, 1200 };
float default_speeds[] = { 5.0, 6.5, 8.5, 12.0, 0.0 };
/* float colors_alpha[][4] = { { 1.0, 0.2, 0.4 , 0.4}, { 0.2, 0.3, 1.0, 0.4}, */
float colors_alpha[][4] = { { .8, 0.1, 0.2 , 0.6}, { 0.856, 0.42, 0.25, 0.6},
		       { 0.1, 1.0, 0.2, 0.6 }, { 0.7, 0.7, 0.7, 0.6 } };
float colors_trail[][4] = { { 1.0, 0.2, 0.4, 1.0 }, { 0.2, 0.3, 1.0, 1.0 },
		       { 0.2, 1.0, 0.5, 1.0 }, { 0.5, 0.5, 0.5, 1.0 } };

float colors_model[][4] = { { 1.0, 1.0, 0.0, 1.0 }, { 1.0, 0.1, 0.1, 1.0 },
		       { 0.3, 1.0, 0.8, 1.0 }, { 0.8, 0.8, 0.8, 1.0 } };

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
float vp_x[4][4] = { { 1 },    { 1, 1 },  { 1, 16, 1, 16 }, { 10 } };
float vp_y[4][4] = { { 1 },    { 0.5, 12.5 },   { 1, 1, 12.5, 12.5 }, { 4.5 } } ;
float vp_w[4][4] = { { 30 },   { 30, 30 }, { 14, 14, 14, 14 }, { 20 } };
float vp_h[4][4] = { { 22.5 }, { 11.5, 11.5 }, { 10.5, 10.5, 10.5, 10.5 }, { 15 } };

/* help message */

/*
char *help[] = { "This is the gltron - Help Screen",
		 "",
		 "Game:",
		 "h     - Toggles this help screen",
		 "space - Starts the game / Pauses the game",
		 "ESC   - Displays Menu",
		 "q     - Quits the game",

		 "Controls:",
		 "a / s        - Player 1: turn left/right",
		 "k / l        - Player 2: turn left/right",
		 "5 / 6        - Player 3: turn left/right",
		 "left / right - Player 4: turn left/right",
		 "View:",
		 "F1 / F2 / F3 - 1, 2 or 4 player view",
		 "F10          - Change Camera movement",
		 
};
*/


