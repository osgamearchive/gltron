/*
  gltron 0.50 beta
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef GLTRON_H
#define GLTRON_H

#define SEPERATOR '/'
#define RC_NAME ".gltronrc"
#define CURRENT_DIR "."

#define SGI_TEX
/* #define SDL_TEX */

/* win32 additions by Jean-Bruno Richard <jean-bruno.richard@mg2.com> */

#ifdef WIN32
#include <windows.h>
#define SOUND
#define M_PI 3.141592654
#undef SEPERATOR
#define SEPERATOR '\\'
#undef RC_NAME
#define RC_NAME "gltron.ini"

#ifdef SGI_TEX
#undef SGI_TEX
#endif

#define SDL_TEX

#endif /* WIN32 */


/* FreeBSD additions by Andrey Zakhatov <andy@icc.surw.chel.su>  */

#ifdef __FreeBSD__
#include <floatingpoint.h>
#endif

/* MacOS additions by Stefan Buchholtz <sbuchholtz@online.de> */

#ifdef macintosh
#include <string.h>
#include <console.h>
#define M_PI 3.141592654
#undef SEPERATOR
#define SEPERATOR ':'
#undef RC_NAME
#define RC_NAME "gltron.ini"
#endif 

/* under windows, use SDL_image to load PNG textures */

#ifdef SGI_TEX
#define TEX_SUFFIX ".sgi"
#endif

#ifdef SDL_TEX
#define TEX_SUFFIX ".png"
#endif

#define COS(X)	cos( (X) * M_PI/180.0 )
#define SIN(X)	sin( (X) * M_PI/180.0 )

/* data structures */
#include "data.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "system.h"

/* use texfont for rendering fonts as textured quads */
/* todo: get rid of that (it's not free) */

/* #include "TexFont.h" */
#include "fonttex.h"

/* menu stuff */

#include "menu.h"

/* TODO(3): incorporate model stuff */
/* model stuff */
#include "model.h"
/* poly-soup stuff */
/* #include "polysoup.h" */

/* do Sound */

#ifdef SOUND
#include "sound.h"
#endif

/* global constants */

#define PLAYERS 4
#define MAX_PLAYERS 4
#define MAX_TRAIL 1000

#define B_HEIGHT 0
#define TRAIL_HEIGHT 3.5
#define CYCLE_HEIGHT 8
#define WALL_H 12

#define CAM_COUNT 3
#define CAM_CIRCLE_DIST 15
#define CAM_CIRCLE_Z 8.0
#define CAM_FOLLOW_DIST 12
#define CAM_FOLLOW_Z 6.0
#define CAM_FOLLOW_SPEED 0.05
#define CAM_FOLLOW_SPEED_FACTOR 1.0 / 82.0
#define CAM_SPEED 2.0

#define EXP_RADIUS_MAX 30
#define EXP_RADIUS_DELTA 0.01

/* these must be < 0 */
#define SPEED_CRASHED -1
#define SPEED_GONE -2

#define FAST_FINISH 40

/* when running as screen saver, wait SCREENSAVER_WAIT ms after each round */

#define SCREENSAVER_WAIT 2000

#define PAUSE_GAME_FINISHED 1

extern int gl_error;

extern settings_int *si;
extern int si_count;
extern settings_float *sf;
extern int sf_count;

extern Game main_game;
extern Game *game;
extern float camAngle;

/* extern TexFont *txf; */
extern fonttex *ftx;
extern int fontID;
#define MAX_FONTS 17

extern Menu** pMenuList;
extern Menu* pRootMenu;
extern Menu* pCurrent;

extern unsigned char* colmap;
extern int colwidth;

extern int dirsX[];
extern int dirsY[];

extern float default_speeds[];
extern int default_arena_sizes[];

extern int lasttime; 
extern double dt; /* milliseconds since last frame */

extern int polycount;
extern int screenshots;

extern float colors_alpha[][4];
extern float colors_trail[][4];
extern float colors_model[][4];
extern int vp_max[];
extern float vp_x[3][4];
extern float vp_y[3][4];
extern float vp_w[3][4];
extern float vp_h[3][4];

/* configure keys stuff */

int *configureKeyEntry;
Menu *configureKeyMenu;

#define KEY_ACTIONS_N 8

typedef struct {
  int player;
  int turn;
  int key;
} keyAction;

extern keyAction key_actions[];
/*
#define HELP_LINES 18
#define HELP_FONT GLUT_BITMAP_9_BY_15
#define HELP_DY 20

extern char *help[];
*/

/* function prototypes */

/* TODO: sort these */
/* engine.c */

extern void setCol(int x, int y, int value, int width, unsigned char *map);
extern int getCol(int x, int y, int width, unsigned char *map);
extern void turn(Data* data, int direction);

extern void idleGame();

extern void initGame();
extern void exitGame();

extern void initGameStructures();

extern void initGameScreen();
extern void initDisplay(gDisplay *d, int type, int p, int onScreen);
extern void changeDisplay();
extern void defaultDisplay(int n);
extern void cycleDisplay(int p);

extern void doTrail(line *t, int value);
extern void fixTrails();
extern void clearTrails(Data *data);

/* gltron.c */

extern void mouseWarp();

extern void initData();
extern void drawGame();
extern void displayGame();
extern void initGLGame();

extern void shutdownDisplay(gDisplay *d);
extern void setupDisplay(gDisplay *d);

extern int colldetect(float sx, float sy, float ex, float ey, int dir, int *x, int *y);

extern int allAI();
extern void setGameIdleFunc(void);
extern void initGlobals(void);
extern int screenSaverCheck(int t);
extern void scaleDownModel(float height, int i);
extern void setMainIdleFunc(void);

/* various initializations -> init.c */

extern void initFonts();

/* texture initializing -> texture.c */

extern void initTexture();
extern void deleteTextures();

/* texture loading -> load_texture.c */
/* uses sgi_texture.c or sdl_texture.c */
extern void loadTexture(char *filename, int format);

/* screenshot.c */
extern void doScreenShot();

/* help -> character.c */

/* extern void drawLines(int, int, char**, int, int); */

/* ai -> computer.c */

extern int freeway(Data *data, int dir);
extern void getDistPoint(Data *data, int d, int *x, int *y);
extern void doComputer(Player *me, Data *him);

/* keyboard -> input.c */

extern void keyGame(int key, int x, int y);
extern void parse_args(int argc, char *argv[]);

/* settings -> settings.c */

extern void initMainGameSettings();
extern void saveSettings();

/* menu -> menu.c */

extern void menuAction(Menu* activated);
extern Menu** loadMenuFile(char* filename);
extern void drawMenu(gDisplay *d);
extern void showMenu();
extern void removeMenu();
extern void initMenuCaption(Menu *m);
extern int* getVi(char *szName);

/* file handling -> file.c */

extern char* getFullPath(char* filename);

/* callback stuff -> switchCallbacks.c */

extern void chooseCallback(char*);
extern void restoreCallbacks();
extern void switchCallbacks(callbacks*);
extern void updateCallbacks();

/* probably common graphics stuff -> graphics.c */

extern void checkGLError(char *where);
extern void rasonly(gDisplay *d);
extern void drawFPS(gDisplay *d);
extern void drawText(int x, int y, int size, char *text);
extern int hsv2rgb(float, float, float, float*, float*, float*);
extern void colorDisc();

/* gltron game graphics -> gamegraphics.c */
extern void drawDebugTex(gDisplay *d);
extern void drawScore(Player *p, gDisplay *d);
extern void drawFloor(gDisplay *d);
extern void drawTraces(Player *, gDisplay *d, int instance);
extern void drawPlayers(Player *);
extern void drawWalls(gDisplay *d);
extern void drawCam(Player *p, gDisplay *d);
extern void drawAI(gDisplay *d);
extern void drawPause(gDisplay *d);
extern void drawHelp(gDisplay *d);

/* font stuff ->fonts.c */
extern void initFonts();
extern void deleteFonts();

extern void resetScores();


extern void draw( void );


extern void chaseCamMove();
extern void timediff();
extern void camMove();

extern void movePlayers();

extern callbacks gameCallbacks;
extern callbacks guiCallbacks;
/* extern callbacks chooseModelCallbacks; */
extern callbacks pauseCallbacks;
extern callbacks configureCallbacks;

#endif
