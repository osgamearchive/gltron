#ifndef DATA_H
#define DATA_H

#include "gltron.h"

/* general data structures */

typedef struct list list;
struct list {
  void *data;
  list* next;
};

typedef struct {
  float x, y;
} Point;

/* game specific data structures */

typedef struct Grid {
  int width, height;
  unsigned char *data;
} Grid;

typedef struct RuleSet {
  int eraseCrashed;
  float speed;
} RuleSet;

typedef struct Time {
  int current;
  int lastFrame;
  int offset; /* from SystemGetElapsedTime() */
  int dt; /* current - lastFrame */

  /* float timeScale; */
} Time;

enum {
  EVENT_TURN_LEFT = 1,
  EVENT_TURN_RIGHT = 2,
  EVENT_CRASH = 4,
  EVENT_STOP = 8
};

enum {
  TURN_LEFT = 3,
  TURN_RIGHT = 1
};

typedef struct GameEvent {
  int type; /* what */
  int player; /* who */
  int x; /* where */
  int y;
  int timestamp;
} GameEvent;
  
enum {
  GAME_SINGLE = 1,
  GAME_SINGLE_RECORD = 2,
  GAME_PLAY = 4,
  GAME_PLAY_NETWORK = 8,
  GAME_NETWORK_RECORD
};

enum {
  NET_EVENT = 'e'
};

typedef struct NetData {
  char type[2];
  char *data;
  int length;
} NetData;

typedef struct Network {
  list *data;
  int status;
} Network;

typedef struct Input {
  int mouse1;
  int mouse2;
  int mousex;
  int mousey;
} Input;
  
typedef struct Game2 {
  Grid grid;
  RuleSet rules;
  int mode;
  int players;
  int *startPositions;
  Time time;
  list events;
  FILE *record;
  FILE *play;
  Network network;
  Input input;
} Game2;
  
typedef struct line {
  int sx, sy, ex, ey;
} line;

typedef struct fontbmp {
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  texture *tex;
} fontbmp;

typedef struct fonttex {
  // sgi_texture **textures;
  int nTextures;
  
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  unsigned int *texID;

  char *fontname;
  char *bitmapname;
  int bitmapTexwidth;
  int bitmapWidth; /* character width */
} fonttex;

typedef struct Model {
  Mesh **mesh; /* models (lod) */
  int lod; /* number of models */
  // int *lod_dist;
  
  float color_alpha[4]; /* alpha trail */
  float color_trail[4]; /* solid edges of trail */
  float color_model[4]; /* model color */
} Model;

typedef struct Data {
  int iposx, iposy;
  float posx, posy;
  float t;
  
  int dir; int last_dir;

  int turn;
  int turn_time; /* for cycle animation */
  
  int score;
  float speed; /* set to -1 when dead */
  float trail_height; /* countdown to zero when dead */
  float exp_radius; /* explosion of the cycle model */
  line *trails;
  line *trail; /* current trail */
} Data;

enum { 
  CAM_FREE_R = 0,
  CAM_FREE_PHI,
  CAM_FREE_CHI 
};

typedef struct CameraMovement {
  float r;
  float phi;
  float chi;
} CameraMovement;

typedef struct CameraType {
  int interpolated_cam;
  int interpolated_target;
  int coupled;
  int freedom[3];
  int type;
} CameraType;

typedef struct Camera {
  float cam[3];
  float target[3];
  CameraMovement *movement;
  CameraType *type;
} Camera;

typedef struct AI {
  int active;
  int tdiff; /*  */
  int moves;
  long lasttime;
  int danger;
  int lastx, lasty;
} AI;

typedef struct {
  char *path;
  char *name;
} Artpack;

typedef struct gDisplay {
  int win_id;     /* nur das globale Window hat eine */
  int h, w;       /* window */
  int vp_x, vp_y; /* viewport */
  int vp_h, vp_w;
  int blending;
  int fog;
  int shademodel;
  int wall;
  int onScreen;

  /* texture ID */
  unsigned int *textures;

  /* software rendering stuff */
  unsigned char *pixelGui;
  fontbmp *bitfont;
  Artpack artpack;
} gDisplay;

typedef struct Player {
  Model *model;
  Data *data;
  Camera *camera;
  gDisplay *display;
  AI *ai;
} Player;

/* if you want to add something and make it permanent (via
   .gltronrc) then
   1) add it to Settings in data.h
   2) add it to settings.txt
   3) add pointer to initSettingData() in settings.c
   4) add a default to initMainGameSettings() in settings.c
   5) make a menu entry in menu.txt
*/

enum {
  BILINEAR = 0,
  TRILINEAR = 1
};

typedef struct Settings {
  /* these settings affect the visuals and sound etc.
     and are client side only */

  int show_help;
  int show_fps;
  int show_wall;
  int show_2d;
  int show_alpha;
  int alpha_trails;
  int light_cycles;

  int show_skybox;
  int show_floor_texture;
  int show_glow;
  int show_scores;
  int show_ai_status;
  int show_model;
  int show_decals; /* disable on G200/G400/win32 due to a driver bug */
  int lod;
  int shadow_lod;
  int show_crash_texture;
  int model_backwards;
  int turn_cycle; /* smooth turning */
  int line_spacing; /* space between lines when the floor texture is
		       disabled */

  int stretch_textures;
  int use_mipmaps; /* enable / disable mipmapping */
  int mipmap_filter; /* bilinear / trilinear */

  int softwareRendering;
  int camType;
  int mouse_warp;

  int display_type; /* 0-2 -> 1, 2 or 4 displays on the screen */
  int content[4]; /* max. 4 individual viewports on the screen */
  int windowMode; /* 0: fullscreen, non-zero: window mode */
  int bitdepth_32; /* 1: 32 bit rendering */

  int fov; /* field ov view (vertical angle) */
  float znear; /* the near z-Plane */
  int width; /* screen width & height */
  int height;

  int playMusic;
  int playEffects;

  float musicVolume;
  float fxVolume;

  list* soundList;
  int soundIndex;
  int soundSongCount;
  /* these two are ignored in multiplayer mode */
  int fast_finish;
  int screenSaver; /* 1: all for players are AIs when the game starts */

  /* no AI in multiplayer yet */
  int ai_player1;
  int ai_player2;
  int ai_player3;
  int ai_player4;
  int ai_level;

  /* these gettings affect the gameplay */
  int erase_crashed;
  int game_speed; /* index */
  float current_speed;
  int grid_size;
  int arena_size; /* index */
} Settings;

typedef struct Game {
  gDisplay *screen;
  Settings *settings;
  Player *player;
  int players; /* number of players - currently limited to 4 somewhere */
  int winner; /* who won this round */
  int pauseflag; /* if the game is finished: the PAUSE_GAME_FINISHED flag
		    is set */
  int running; /* the amount of players that are still alive */
} Game;

typedef struct settings_int {
  char name[32];
  int *value;
} settings_int;

typedef struct settings_float {
  char name[32];
  float *value;
} settings_float;


typedef struct settings_v {
  char name[32];
  void (*value)(char*, FILE*);
} settings_v;

#endif
