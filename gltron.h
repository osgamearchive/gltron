/*
  gltron
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

#ifndef M_PI
#define M_PI 3.141592654
#endif
/* win32 additions by Jean-Bruno Richard <jean-bruno.richard@mg2.com> */

#ifdef WIN32
#include <windows.h>

#undef SEPERATOR
#define SEPERATOR '\\'
#undef RC_NAME
#define RC_NAME "gltron.ini"
#define CURRENT_EQ_DATA_DIR 

#endif /* WIN32 */

/* FreeBSD additions by Andrey Zakhatov <andy@icc.surw.chel.su>  */

#ifdef __FreeBSD__
#include <floatingpoint.h>
#endif

/* MacOS additions by Stefan Buchholtz <sbuchholtz@online.de> */

#ifdef macintosh
#include <string.h>
#include <console.h>
#undef SEPERATOR
#define SEPERATOR ':'
#undef RC_NAME
#define RC_NAME "gltron.ini"
#define CURRENT_EQ_DATA_DIR 
#undef CURRENT_DIR
#define CURRENT_DIR ""
#endif 

/* dropped support for anything else than libpng */
#include "png_texture.h"
#include <png.h>
typedef png_texture texture;
#define LOAD_TEX(x) load_png_texture(x)
#define TEX_SUFFIX ".png"

#define COS(X)	cos( (X) * M_PI/180.0 )
#define SIN(X)	sin( (X) * M_PI/180.0 )

/* data structures */
#include "model.h"
#include "data.h"
#include "menu.h"
#include "quad.h"
#include "callbacks.h"

#include "system.h"
#include "geom.h"
#include "light.h"

/* sound stuff */
#include "sound3d.h"
/* rendering stuff */
#include "renderer_gl.h"

/* resource management */
#include "texture.h"

extern int game_textures;
extern int n_textures;
extern texture_info textures[];

/* artpack stuff */

extern void getArtPath(char *texname, char *fullpath);
extern void initArtpacks(void);
extern void loadArt(void);
extern void reloadArt(void);

/* graphics libs */
#include <GL/gl.h>

/* do Sound */
#ifdef SOUND
#include "sound.h"
#endif

/* use enums, not magic */

enum { 
  AI_HUMAN = 0,
  AI_COMPUTER = 1,
  AI_NONE = 2
  
};

/* global constants */

/* physics recomputed every 10ms */
#define PHYSICS_RATE 20

#define SONG_PREFIX "song_"
#define MUSIC_DIR "music"

#define PATH_ART "art"

#define PLAYERS 4
#define MAX_PLAYERS 4
#define MAX_TRAIL 1000

#define B_HEIGHT 0
#define TRAIL_HEIGHT 3.5
#define CYCLE_HEIGHT 8
#define RECOGNIZER_HEIGHT 20
#define WALL_H 12

#define CAM_TYPE_CIRCLING 0
#define CAM_TYPE_FOLLOW 1
#define CAM_TYPE_COCKPIT 2
#define CAM_TYPE_MOUSE 3

#define CAM_COUNT 4
#define CAM_CIRCLE_DIST 17
#define CAM_CIRCLE_Z 8.0

#define CAM_FOLLOW_DIST 18
#define CAM_FOLLOW_Z 6.0
#define CAM_FOLLOW_SPEED 0.05
#define CAM_FOLLOW_SPEED_FACTOR 1.0 / 82.0
#define CAM_SPEED 0.000349

#define CAM_COCKPIT_Z 3

#define CAM_R_MIN 2.0
#define CAM_R_MAX 100
#define CAM_CHI_MIN M_PI / 6
#define CAM_CHI_MAX M_PI / 2 - M_PI / 6

#define CAM_DR 6.4

#define MOUSE_ORIG_X 100
#define MOUSE_ORIG_Y 100

#define MOUSE_CX 0.003
#define MOUSE_CY 0.003

#define TURN_LENGTH 200

#define EXP_RADIUS_MAX 30
#define EXP_RADIUS_DELTA 0.01

/* these must be < 0 */
#define SPEED_CRASHED -1
#define SPEED_GONE -2

#define FAST_FINISH 40

#define DEBUG_TEX_W 64
#define DEBUG_TEX_H 64
/* when running as screen saver, wait SCREENSAVER_WAIT ms after each round */

#define SCREENSAVER_WAIT 2000

#define PAUSE_GAME_FINISHED 1

#ifdef __NETWORK__
#include <SDL_net.h>
#include "protocol.h"
#include "netevent.h"
#include "wtext.h"
#include "wprogress.h"
#include "splash.h"
#endif

extern char** artpack_list;
extern int artpack_index;

extern int gl_error;

extern char messages[80];

extern settings_int *si;
extern int si_count;
extern settings_float *sf;
extern int sf_count;
extern settings_v *sv;
extern int sv_count;

extern Game main_game;
extern Game *game;
extern Game2 main_game2;
extern Game2 *game2;

extern float camAngle;
extern float cam_phi;
extern float cam_chi;
extern float cam_r_mouse;
extern float cam_r_follow;
extern float cam_r_circle;

#ifdef DEPTH_SORT
extern int quadBufSize;
extern Quad* quadBuf;
extern int* quadBufIndex;
#endif

extern Mesh* recognizer;

/* extern TexFont *txf; */
extern fonttex *gameFtx;
extern fonttex *guiFtx;
extern int fontID;
#define MAX_FONTS 17

extern Menu** pMenuList;
extern Menu* pRootMenu;
extern Menu* pCurrent;

extern unsigned char* colmap;
extern unsigned char* debugtex;
extern int ogl_debugtex;
extern int colwidth;

extern int dirsX[];
extern int dirsY[];
extern float camAngles[];

extern float default_speeds[];
extern int default_arena_sizes[];

extern int stoptime;
/*
extern int lasttime; 
extern double dt; 
*/
/* milliseconds since last frame */

extern int polycount;
extern int screenshots;

extern float colors_alpha[][4];
extern float colors_trail[][4];
extern float colors_model[][4];
extern int vps;
extern int vp_max[];
extern float vp_x[4][4];
extern float vp_y[4][4];
extern float vp_w[4][4];
extern float vp_h[4][4];
extern unsigned char debugcolors[6][4];

extern float shadow_color[4];
extern float shadow_matrix[16];

/* configure keys stuff */

extern int *configureKeyEntry;
extern Menu *configureKeyMenu;

extern callbacks gameCallbacks;
extern callbacks guiCallbacks;
/* extern callbacks chooseModelCallbacks; */
extern callbacks pauseCallbacks;
extern callbacks configureCallbacks;
extern callbacks promptCallbacks;
extern callbacks creditsCallbacks;
#ifdef __NETWORK__
extern callbacks netConnectCallbacks;
extern callbacks nicknameCallbacks;
extern callbacks serverCallbacks;
extern callbacks portCallbacks;
extern callbacks netPregameCallbacks;
extern callbacks keyboardreadingCallbacks;
extern callbacks scoresCallbacks;
extern callbacks netWaitCallbacks;
#endif

#define KEY_ACTIONS_N 8

typedef struct {
  int player;
  int turn;
  int key;
} keyAction;

extern keyAction key_actions[];

/* function prototypes */
/* TODO: sort these */

/* console */
void consoleInit();
void consoleAddLine(char *text);
void consoleDisplay(void(*func)(char *line, int call), int height);
void consoleScrollForward(int range);
void consoleScrollBackward(int range);

/* record.c */

extern void getEvents();
extern void writeEvent(GameEvent *e);
extern int startRecording(char *filename);
extern void stopRecording();
extern int startPlaying(char *filename);
extern void stopPlaying();

/* engine.c */

extern void addList(list **l, void *data);
extern int updateTime();
extern int getCol(int x, int y);
extern void turn(Data* data, int direction);
extern void idleGame();
extern void initGame();
extern void exitGame();
extern void initGameStructures();
extern void initGameScreen();
extern void defaultDisplay(int n);
extern void cycleDisplay(int p);
extern void doTrail(line *t, int value);
extern void fixTrails();
extern void clearTrails(Data *data);

extern void moveStep(Data* data);
extern void writePosition(int player);

extern void doTurn(Data* data, int time);
extern void crashPlayer(int player);
extern void createTurnEvent(int player, int direction);
extern int processEvent(GameEvent *e);

/* gltron.c */
/* game.c */
/* gamegraphics.c */

extern void initClientData();
extern void initDisplay(gDisplay *d, int type, int p, int onScreen);
extern void changeDisplay();
extern void initModel(Player *p, int p_num);
extern void initModelLights(int light);

extern void gameMouseMotion(int x, int y);
extern void gameMouse(int buttons, int state, int x, int y);

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

/* skybox.c */
extern void skybox(void);

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
extern void doBmpScreenShot();

/* help -> character.c */

/* extern void drawLines(int, int, char**, int, int); */

/* ai -> computer.c */

extern void doComputer(int player, int target);
extern void doComputer2(int player, int target);

/* keyboard -> input.c */

extern void keyGame(int key, int unicode, int x, int y);
extern void parse_args(int argc, char *argv[]);

/* settings -> settings.c */

extern void loadIniFile(char *name);
extern void initMainGameSettings();
extern void initDefaultSettings();
extern void saveSettings();

/* menu -> menu.c */

extern void menuAction(Menu* activated, int type);
extern Menu** loadMenuFile(char* filename);
extern void drawMenu(gDisplay *d);
extern void showMenu();
extern void removeMenu();
extern void initMenuCaptions();
extern void initMenuCaption(Menu *m);
extern int* getVi(char *szName);
extern float* getVf(char *szName);

/* file handling -> file.c */

extern char* getFullPath(char *filename);
extern list* readDirectoryContents(char *dirname, char *prefix);
extern char* getMusicPath(char *dirname);

/* findpath.c, GPL'd code */
extern void goto_installpath(char *executable);

/* callback stuff -> switchCallbacks.c */

#include "switchCallbacks.h"


/* probably common graphics stuff -> graphics.c */

extern void checkGLError(char *where);
extern void rasonly(gDisplay *d);

//extern void drawText(fonttex* ftx, int x, int y, int size, char *text);
extern void drawText(fonttex* ftx, float x, float y, int size, char *text);
extern int hsv2rgb(float, float, float, float*, float*, float*);
extern void colorDisc();

/* gltron game graphics -> gamegraphics.c */
extern void rebuildDebugTex();
extern void drawDebugLines(gDisplay *d);
extern void drawDebugTex(gDisplay *d);
extern void drawFPS(gDisplay *d);
extern void drawConsole(gDisplay *d);
extern void drawScore(Player *p, gDisplay *d);
extern void drawAI(gDisplay *d);
extern void drawPause(gDisplay *d);
/* extern void drawHelp(gDisplay *d); */
extern void drawFloor(gDisplay *d);
extern void drawPlayers(Player *);
extern void drawWalls(gDisplay *d);
extern void drawCam(Player *p, gDisplay *d);

/* trail.c */
/* extern void drawTrails(Player *p, Player *p_eye, gDisplay *d); */
extern void drawTrailBow(Player *p, int flag);
extern void drawTrailLines(Player *p);
extern void doTrails(Player *p);
extern void drawTrailShadow(Player *p);
extern void drawTrailQuadBow(Player *p, int *q);
extern void drawTrailsWithQuadBuf(Player *p_eye);
extern float getSegmentUV(line *line);
extern float getSegmentEndUV(line *line, Data *data);
extern float getSegmentEndX(line *line, Data *data, int type);
extern float getSegmentEndY(line *line, Data *data, int type);
extern void checkQuad2D(char *flags, int q, int n);

/* clip.c */
int testfrustum(float *x, float *y, float *p, float *a, float *b);

/* recognizer stuff */
extern void recognizerMovement();
extern void drawRecognizers(int flag);

/* demo stuff */
/* record.c */
extern int readDemoInfo();
extern int writeDemoInfo();

/* engine.c */
extern int applyGameInfo();


/* pixel stuff */
extern unsigned char* loadPixels(char *filename, gDisplay *d);
extern unsigned char* scalePixels(unsigned char *source, int sw, int sh,
				  int x, int y, int w, int h,
				  int dw, int dh, int bytes);

/* font stuff ->fonts.c */
extern void initFonts();
extern void deleteFonts();
extern void resetScores();

extern void draw( void );

#include "camera.h"

extern void doCameraMovement();
extern void initCamera(Camera *cam, Data *data, int type);

extern void movePlayers();

/* fonttex stuff */
extern void initBitmaps(gDisplay *d);
extern void deleteBitmaps(gDisplay *d);
extern fontbmp* fbmpLoadFont(char *filename);
extern void fbmpUnloadFont(fontbmp *fbmp);
extern texture* loadTextureData(char *filename);
extern void freeTextureData(texture *tex);
extern void drawSoftwareText(fonttex *tex, int x, int y, int size, char *text);
extern void drawSoftwareHighlight(int x, int y, int size, char *text);
extern fonttex *ftxLoadFont(char *filename);
extern void ftxUnloadFont(fonttex *ftx);
extern void loadTexture(char *filename, int format);

/* It's not necessary to bind the texture explicitly. */
/* (and we don't know which texture to bind) */

/* ftxRenderString will take care of that */
/* extern void ftxBindFontTexture(fontTex *ftx); */

extern void ftxRenderString(fonttex *ftx, char *string, int len);

/* extern void ftxGetStringWidth(fontTex *ftx, */
/*                               char *string, int len, int *width); */
/* can't get max_ascent, max_descent yet */

#ifdef __NETWORK__
extern Slots  slots[MAX_PLAYERS]; //slots of the server.
extern int    nbUsers; 
extern int    serverstate;
extern char   name[255];
extern int    isLogged;
extern int    sChat;
extern char   mesg[255];
extern int    me;

extern netEventList neteventlist;

extern int    packet_type;
extern int    timeout;
extern int    hasstarted;
extern char   server_message[255];
extern Netscores         netscores;

extern Wtext    *pregametext;
extern Wintext  *inpregametext;

extern Splash   *splash;

extern void  handleServer      ( void );
extern void  login             ( char* );
extern void  drawMessage       ( char* );
extern void  displayPregame    ( void );
extern void  drawChat          ( char* );
extern char* getInputEntry     ( void );
extern void  drawGuiBackground ( void );
extern void  drawGuiLogo       ( void );
extern int   getping           ( void );
extern void  makeping          ( int  );

extern void  createTurnList    ( void );
extern void  idleTurns         ( void );
extern void  initTurnList      ( void );
extern void  freeTurnList      ( void );
extern void  doPredictedTurn   ( int, int );
extern int   get_size_predictedturn();
extern int   undoTurn          ( int, int, int );
#ifdef USEUDP
extern void  handleUDP         ( void );
#endif
#endif

#endif


