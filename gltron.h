#ifndef GLTRON_H
#define GLTRON_H

/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

	/* portable random numbers */
#include "random.h"

	/* basic io wrapper */
#include "file_io.h"

#define SEPERATOR '/'
#define RC_NAME ".gltronrc"

#ifndef M_PI
#define M_PI 3.141592654
#endif

	/* Win32 port maintained by Andreas Umbach <marvin@dataway.ch> */
#ifdef WIN32
#include <windows.h>

#undef SEPERATOR
#define SEPERATOR '\\'
#undef RC_NAME
#define RC_NAME "gltron.ini"

#endif /* WIN32 */

	/* FreeBSD additions by Andrey Zakhatov <andy@icc.surw.chel.su>  */

#ifdef __FreeBSD__
#include <floatingpoint.h>
#endif

/* MacOS port maintained by Darrell Walisser <walisser@mac.com> */

#ifdef macintosh 
#include <string.h>
#undef SEPERATOR
#define SEPERATOR ':'
#undef RC_NAME
#define RC_NAME "gltronPrefs.txt"
#endif 

#ifdef __APPLE__
#undef RC_NAME
#define RC_NAME "gltronPrefs.txt"
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
#include "event.h"

#include "system.h"
#include "geom.h"
#include "light.h"

#include "recognizer.h"

	/* rendering stuff */
#include "renderer_gl.h"

	/* resource management */
#include "texture.h"

#include "console.h"

	/* scripting */
#include "scripting.h"
#include "settings.h"

	extern int game_textures;
	extern int n_textures;
	extern texture_info textures[];

	/* artpack stuff */

	extern void initArtpacks(void);
	extern void loadArt(void);
	extern void reloadArt(void);

	/* graphics libs */
	// #include "extgl.h"
#include <GL/gl.h>
	// #include <GL/glext.h>

	/* do Sound */
#ifdef SOUND
#include "Sound.h"
#include "sound_glue.h"
#endif

	/* use enums, not magic */

	enum { 
		AI_HUMAN = 0,
		AI_COMPUTER = 1,
		AI_NONE = 2
	};

	enum {
		RETURN_GAME_LAUNCH = 0,
		RETURN_GAME_END,
		RETURN_GAME_PAUSE,
		RETURN_GAME_UNPAUSE,
		RETURN_GAME_CREDITS,
		RETURN_GAME_ESCAPE,
		RETURN_GUI_PROMPT,
		RETURN_TIMEDEMO,
		RETURN_TIMEDEMO_ABORT,
		RETURN_CREDITS,
		RETURN_QUIT
	};
	/* global constants */

	/* physics recomputed every 10ms */
#define PHYSICS_RATE 20

#define SONG_PREFIX "song_"

#define PLAYERS 4
#define MAX_PLAYERS 4
#define MAX_TRAIL 1000

#define B_HEIGHT 0
#define TRAIL_HEIGHT 3.5
#define CYCLE_HEIGHT 8
#define RECOGNIZER_HEIGHT 50
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

#define CAM_COCKPIT_Z 4 

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

	extern int gl_error;

	extern Game main_game;
	extern Game *game;
	extern Game2 main_game2;
	extern Game2 *game2;

	extern int viewport_content[4];
	extern void (*ai_function)(int,int);

#ifdef LUA_PROFILE
	extern int lua_profile;
#endif

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

#define LC_LOD 3
	extern char *lc_lod_names[];
	extern Mesh *recognizer;
	extern Mesh *recognizer_quad;
	extern float rec_outline_color[3];
	extern float rec_spec_color[4];

	extern Mesh *lightcycle[];

	/* extern TexFont *txf; */
	extern fonttex *gameFtx;
	extern fonttex *guiFtx;
	extern int fontID;
#define MAX_FONTS 17

	extern unsigned char* colmap;
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

	extern float colors_trail_diffuse[][4];
	extern float colors_model_diffuse[][4];
	extern float colors_model_specular[][4];
	extern int vps;
	extern int vp_max[];
	extern float vp_x[4][4];
	extern float vp_y[4][4];
	extern float vp_w[4][4];
	extern float vp_h[4][4];
	extern unsigned char debugcolors[6][4];

	extern float shadow_color[4];
	extern float shadow_matrix[16];

	extern callbacks gameCallbacks;
	extern callbacks guiCallbacks;
	extern callbacks pauseCallbacks;
	extern callbacks configureCallbacks;
	extern callbacks promptCallbacks;
	extern callbacks creditsCallbacks;
	extern callbacks timedemoCallbacks;

	/* function prototypes */
	/* TODO: sort these */

	/* record.c */

	extern void getEvents();
	extern void writeEvent(GameEvent *e);
	extern int startRecording(char *filename);
	extern void stopRecording();
	extern int startPlaying(char *filename);
	extern void stopPlaying();

	/* engine.c */

	extern void addList(list **l, void *data);
	extern void updateSettingsCache();
	extern int updateTime();
	extern int getCol(int x, int y);
	extern void turn(Data* data, int direction);
	extern void idleGame();
	extern void initGame();
	extern void exitGame();
	extern void initGameStructures();
	extern void initGameScreen();
	extern void doTrail(Line *t, int value);
	extern void fixTrails();
	extern void clearTrails(Data *data);

	extern list* doMovement(int mode, int dt);
	extern void moveStep(Data* data);
	extern void writePosition(int player);

	/* gltron.c */
	/* game.c */
	/* gamegraphics.c */

	extern void initClientData();
	extern void initDisplay(gDisplay *d, int type, int p, int onScreen);
	extern void changeDisplay(int view);
	extern void updateDisplay(ViewportType VPtype); /* vp types defined in data.h */
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

	/* various initializations -> init.c */

	extern void initFonts();

	/* texture initializing -> texture.c */

	extern void initTexture(gDisplay*);
	extern void deleteTextures(gDisplay*);

	/* texture loading -> load_texture.c */
	/* uses sgi_texture.c or sdl_texture.c */
	extern void loadTexture(const char *filename, int format);

	/* screenshot.c */
	extern void doBmpScreenShot(gDisplay *display);
	extern void doPngScreenShot(gDisplay *display);

	/* help -> character.c */

	/* extern void drawLines(int, int, char**, int, int); */

	/* ai -> computer.c */

	extern void initGameAI(int level);
	extern void doComputer(int player, int target);
	extern void doComputer2(int player, int target);

	/* keyboard -> input.c */

	extern void keyGame(int key, int x, int y);
	extern void parse_args(int argc, char *argv[]);

	/* settings -> settings.c */

	extern void initMainGameSettings();
	extern void initDefaultSettings();
	extern void initColors();
	extern void checkSettings();
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
#include "file.h"

	extern list* readDirectoryContents(const char *dirname, char *prefix);

	/* findpath.c, GPL'd code */
	extern void goto_installpath(char *executable);

	/* callback stuff -> switchCallbacks.c */

#include "switchCallbacks.h"


	/* probably common graphics stuff -> graphics.c */

	extern void checkGLError(char *where);
	extern void rasonly(gDisplay *d);

	extern void drawText(fonttex* ftx, int x, int y, int size, char *text);
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
	extern void drawPlayers(Player *);
	extern void  drawWalls();
	extern void drawCam(Player *p, gDisplay *d);

	extern void draw2D( gDisplay *d );
		
	/* trail.c */
	/* extern void drawTrails(Player *p, Player *p_eye, gDisplay *d); */
	extern void drawTrailBow(Player *p, int flag);
	extern void drawTrailLines(Player *p);
	extern void doTrails(Player *p);
	extern void drawTrailShadow(Player *p);
	extern void drawTrailQuadBow(Player *p, int *q);
	extern void drawTrailsWithQuadBuf(Player *p_eye);
	extern float getSegmentUV(Line *line);
	extern float getSegmentEndUV(Line *line, Data *data);
	extern float getSegmentEndX(Line *line, Data *data, int type);
	extern float getSegmentEndY(Line *line, Data *data, int type);
	extern void checkQuad2D(char *flags, int q, int n);

	/* clip.c */
	int testfrustum(float *x, float *y, float *p, float *a, float *b);

	/* demo stuff */
	/* record.c */
	extern int readDemoInfo();
	extern int writeDemoInfo();

	/* engine.c */
	extern int applyGameInfo();


	/* pixel stuff */
	extern unsigned char* loadPixels(const char *filename, gDisplay *d);
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
	extern texture* loadTextureData(const char *filename);
	extern void freeTextureData(texture *tex);
	extern fonttex *ftxLoadFont(char *filename);
	extern void ftxUnloadFont(fonttex *ftx);
	extern void loadTexture(const char *filename, int format);

	/* It's not necessary to bind the texture explicitly. */
	/* (and we don't know which texture to bind) */

	/* ftxRenderString will take care of that */
	/* extern void ftxBindFontTexture(fontTex *ftx); */

	extern void ftxRenderString(fonttex *ftx, char *string, int len);

	/* extern void ftxGetStringWidth(fontTex *ftx, */
	/*                               char *string, int len, int *width); */
	/* can't get max_ascent, max_descent yet */

	/* macFuncs.c macintosh-specific (Mac OS 9/Classic) functions */
#ifdef macintosh
	void setupHomeEnvironment();
#endif

	/* menu sound enumerations */
	enum {
		fx_action,
		fx_highlight
	};

	extern int joy_threshold;
	
#ifdef __cplusplus
}
#endif /* extern C */

#endif /* GLTRON_H */
