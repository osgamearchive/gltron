#ifndef VIDEO_H
#define VIDEO_H

#include "game/camera.h"
#include "video/fonttex.h"
#include "video/model.h"

#include "Nebu_video.h"

/* dropped support for anything else than libpng */
#include <png.h>
typedef png_texture texture;
#define LOAD_TEX(x) load_png_texture(x)
#define TEX_SUFFIX ".png"

typedef struct {
  char *path;
  /* char *name; currently ignored */
} Artpack;

typedef struct {
	Visual display;

	float pColorDiffuse[4];
	float pColorSpecular[4];
	float pColorAlpha[4];

  unsigned int turn_time; /* for cycle animation */
  unsigned int spoke_time; /* for cycle wheel animation */
  int spoke_state; /* showing spoke or not */

  /* explosion stuff */
  float impact_radius;
  float exp_radius; /* explosion of the cycle model */
} PlayerVisual;

enum { VP_SINGLE = 0, VP_SPLIT = 1, VP_FOURWAY = 2 }; // Viewport Type;

#include "recognizer.h"

/* rendering stuff */
#include "explosion.h"

#include "graphics_utility.h"
#include "graphics_fx.h"
#include "graphics_hud.h"
#include "graphics_lights.h"
#include "graphics_world.h"
#include "trail_geometry.h"

/* resource management */
#include "texture.h"

extern int game_textures;
extern int n_textures;
extern TextureInfo textures[];

/* artpack stuff */

extern void initArtpacks(void);
extern void loadArt(void);
extern void reloadArt(void);

/* graphics libs */
// #include "extgl.h"
#include <GL/gl.h>
// #include <GL/glext.h>

enum {
	POINT = 0,
  LINEAR,
	MIPMAP,
  TRILINEAR
};



#define B_HEIGHT 0
#define CYCLE_HEIGHT 8
#define RECOGNIZER_HEIGHT 50
#define WALL_H 12

/* global constants */

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

#define EXP_RADIUS_MAX 30
#define EXP_RADIUS_DELTA 0.01

/* trail definitions */
#define DECAL_WIDTH 20.0
#define BOW_LENGTH 6

#define BOW_DIST3 2
#define BOW_DIST2 0.85
#define BOW_DIST1 0.4


extern int gl_error;


extern int viewport_content[4];


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
extern FontTex *gameFtx;
extern FontTex *guiFtx;
extern int fontID;
#define MAX_FONTS 17

extern float camAngles[];

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


/* pixel stuff */
extern unsigned char* loadPixels(const char *filename, Visual *d);
extern unsigned char* scalePixels(const unsigned char *source, 
																	int sw, int sh,
																	int x, int y, int w, int h,
																	int dw, int dh, int bytes);

/* font stuff ->fonts.c */
extern void initFonts();
extern void deleteFonts();
extern void draw( void );

extern void doCameraMovement();
extern void initCamera(Camera *cam, Data *data, int type);
/* FontTex stuff */
extern texture* loadTextureData(const char *filename);
extern void freeTextureData(texture *tex);
extern FontTex *ftxLoadFont(const char *filename);
extern void ftxUnloadFont(FontTex *ftx);
extern void loadTexture(const char *filename, int format);

/* It's not necessary to bind the texture explicitly. */
/* (and we don't know which texture to bind) */

/* ftxRenderString will take care of that */
/* extern void ftxBindFontTexture(fontTex *ftx); */

extern void ftxRenderString(FontTex *ftx, const char *string, int len);

/* extern void ftxGetStringWidth(fontTex *ftx, */
/*                               const char *string, int len, int *width); */
/* can't get max_ascent, max_descent yet */

extern void doTrail(Line *t, int value);

extern void initDisplay(Visual *d, int type, int p, int onScreen);
extern void changeDisplay(int view);
extern void updateDisplay(int vpType); 
/* vp types defined in data.h */

extern void drawGame();
extern void displayGame();
extern void initGLGame();

extern void shutdownDisplay(Visual *d);
extern void setupDisplay(Visual *d);

extern void initFonts();
extern void initTexture(Visual*);
extern void deleteTextures(Visual*);

/* texture loading -> load_texture.c */
/* uses sgi_texture.c or sdl_texture.c */
extern void loadTexture(const char *filename, int format);

/* screenshot.c */
extern void doBmpScreenShot(Visual *display);
extern void doPngScreenShot(Visual *display);

/* probably common graphics stuff -> graphics.c */

extern void rasonly(Visual *d);

extern int hsv2rgb(float, float, float, float*, float*, float*);
extern void colorDisc();

/* gltron game graphics -> gamegraphics.c */
extern void rebuildDebugTex();
extern void drawDebugLines(Visual *d);
extern void drawDebugTex(Visual *d);
/* extern void drawHelp(Visual *d); */
extern void drawPlayers(Player *p, PlayerVisual *pV);
extern void drawCam(Player *p, PlayerVisual *pV);

extern void draw2D( Visual *d );
		
/* trail.c */
extern void drawTrailLines(Player *p, PlayerVisual *pV);
extern void drawTrailShadow(Player *p, PlayerVisual *pV);
extern float getSegmentUV(Line *line);
extern float getSegmentEndUV(Line *line, Data *data);
extern float getSegmentEndX(Data *data, int type);
extern float getSegmentEndY(Data *data, int type);

extern void drawMenu(Visual *d);

extern void initVideoData();
extern void initGameScreen();

extern void Video_Idle();

extern Visual *gScreen;
extern int gViewportType;
extern int video_initialized;

extern PlayerVisual *gPlayerVisuals;

#endif
