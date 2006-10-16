#ifndef VIDEO_H
#define VIDEO_H

#define NEW_LEVEL_DRAW

#include "game/game_data.h" // Player
#include "game/camera.h" // Camera
#include "video/texture.h" // Texture
#include "video/nebu_video_types.h" // Visual
#include "video/nebu_2d.h"
#include "video/nebu_font.h"
#include "video/video_level.h"
#include "video/model.h" // gltron_Mesh
#include "base/nebu_png_texture.h" // png_texture

/* dropped support for anything else than libpng */
typedef png_texture texture;
#define LOAD_TEX(x) load_png_texture(x)
#define TEX_SUFFIX ".png"

typedef struct Artpack {
  char *path;
  /* char *name; currently ignored */
} Artpack;

typedef struct PlayerVisual {
	Camera camera;

	Visual display;

	float pColorDiffuse[4];
	float pColorSpecular[4];
	float pColorAlpha[4];

  // unsigned int turn_time; /* for cycle animation */
  unsigned int spoke_time; /* for cycle wheel animation */
  int spoke_state; /* showing spoke or not */
} PlayerVisual;

enum { VP_SINGLE = 0, VP_SPLIT = 1, VP_FOURWAY = 2 }; // Viewport Type;

extern TextureInfo textures[];

/* artpack stuff */

void initArtpacks(void);
void loadArt(void);
void loadModels(void);

enum {
	NEAREST = 0,
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
#define CAM_TYPE_OFFSET 4

#define CAM_COUNT 5
#define CAM_CIRCLE_DIST 17
#define CAM_CIRCLE_Z 8.0f

#define CAM_FOLLOW_DIST 18
#define CAM_FOLLOW_Z 6.0f
#define CAM_FOLLOW_SPEED 0.05f
#define CAM_FOLLOW_SPEED_FACTOR 1.0f / 82.0f
#define CAM_SPEED 0.000349f

#define CAM_COCKPIT_Z 4 

#define CAM_R_MIN 2.0
#define CAM_R_MAX 100
#define CAM_CHI_MIN PI / 6
#define CAM_CHI_MAX PI / 2 - PI / 6

#define CAM_DR 6.4f

#define EXP_RADIUS_MAX 30
#define EXP_RADIUS_DELTA 0.01f

/* trail definitions */
#define DECAL_WIDTH 20.0f
#define BOW_LENGTH 6

#define BOW_DIST3 2
#define BOW_DIST2 0.85f
#define BOW_DIST1 0.4f


extern int gl_error;


extern int viewport_content[4];


extern float camAngle;
extern float cam_phi;
extern float cam_chi;
extern float cam_r_mouse;
extern float cam_r_follow;
extern float cam_r_circle;

#define LC_LOD 3
extern char *lc_lod_names[];

extern int gTokenRecognizer;
extern int gTokenRecognizerQuad;
extern int gpTokenLightcycles[LC_LOD];
extern int gpTokenCurrentLevel;
extern int gpTokenCurrentFloor;
// extern gltron_Mesh *recognizer;
// extern gltron_Mesh *recognizer_quad;
// extern gltron_Mesh *lightcycle[];

extern float rec_outline_color[3];
extern float rec_spec_color[4];


extern video_level *gWorld;

extern int gTokenGameFont;
// extern nebu_Font *gameFtx;

enum { 
	eHUDSpeed = 0,
	eHUDMaskSpeed,
	eHUDMaskTurbo,
	eHUDAIStatus,
	eHUDMap,
	eHUDScores,
	eHUDFPS,
	eHUDBuster,
	eHUDMaskBuster,
	eHUDElementCount
};

extern int gpTokenHUD[];

extern float camAngles[];

extern int gIsRenderingReflection;

extern float colors_trail_diffuse[][4];
extern float colors_model_diffuse[][4];
extern float colors_model_specular[][4];
extern int vps;
extern int vp_max[];
// viewports, in virtual units
extern float vp_x[4][4];
extern float vp_y[4][4];
extern float vp_w[4][4];
extern float vp_h[4][4];
extern unsigned char debugcolors[6][4];

extern float gShadowColor[4];
extern float gCurrentShadowColor[4];
extern float shadow_matrix[16];


/* pixel stuff */
extern unsigned char* loadPixels(const char *filename, Visual *d);
extern unsigned char* scalePixels(const unsigned char *source, 
	int sw, int sh,
	int x, int y, int w, int h,
	int dw, int dh, int bytes);

/* font stuff ->fonts.c */
extern void initFonts(void);
extern void draw( void );

extern texture* loadTextureData(const char *filename);
extern void freeTextureData(texture *tex);
extern void loadTexture(const char *filename, int format);

extern void doTrail(segment2 *t, int value);

extern void initDisplay(Visual *d, int type, int p, int onScreen);
extern void changeDisplay(int view);
extern void updateDisplay(int vpType); 
/* vp types defined in data.h */

extern void drawGame(void);
extern void displayGame(void);

extern void shutdownDisplay(void);
extern void setupDisplay(void);

extern void initTexture(Visual*);

/* texture loading -> load_texture.c */
/* uses sgi_texture.c or sdl_texture.c */
extern void loadTexture(const char *filename, int format);

/* screenshot.c */
extern void doBmpScreenShot(Visual *display);
extern void doPngScreenShot(Visual *display);

/* probably common graphics stuff -> graphics.c */

extern void rasonly(Visual *d);

extern int hsv2rgb(float, float, float, float*, float*, float*);
extern void colorDisc(void);

/* gltron game graphics -> gamegraphics.c */
extern void rebuildDebugTex(void);
extern void drawDebugLines(Visual *d);
extern void drawDebugTex(Visual *d);
/* extern void drawHelp(Visual *d); */
extern void drawPlayers(int player);
extern void drawCam(int player);
extern void drawSharpEdges(gltron_Mesh *pMesh);

extern void draw2D( nebu_Rect *pRect );
		
/* trail.c */
extern void drawTrailLines(Player *p, PlayerVisual *pV);
extern void drawTrailShadow(Player *p, PlayerVisual *pV);
extern float getSegmentUV(segment2 *line);
extern float getSegmentEndUV(segment2 *line, Data *data);
extern float getSegmentEndX(Data *data, int type);
extern float getSegmentEndY(Data *data, int type);

extern void freeVideoData(void);
extern void initVideoData(void);
extern void initGameScreen(void);
extern void gui_LoadResources(void);
extern void gui_ReleaseResources(void);
extern void video_LoadResources(void);
extern void video_ReleaseResources(void);

extern void Video_Idle(void);

extern Visual *gScreen;
extern int gViewportType;
extern int video_initialized;

extern PlayerVisual *gPlayerVisuals;

#endif
