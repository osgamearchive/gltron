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

/* 
   this struct contains all the necessary parameters to define a game round
   (except number of players)
   any change forces a restart of the round 
*/

typedef struct RuleSet {
  int eraseCrashed;
  float speed;
  int grid_size;
  int ai_level;
} RuleSet;

typedef struct Time {
  unsigned int current;
  unsigned int lastFrame;
  unsigned int offset; /* from SystemGetElapsedTime() */
  unsigned int dt; /* current - lastFrame */

  /* float timeScale; */
} Time;
  
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

typedef struct DisplayList {
  GLuint list_start;
  GLuint floor_list;
  GLuint skybox_list;
} DisplayList;

/* 
 * SettingsCache - A Structure for storing lua settings during play to 
 * avoid the overhead of fetching from lua repeatedly. Values should be 
 * considered and const as they are never written back out to lua.
 */
typedef struct SettingsCache {
  int use_stencil;
  int show_scores;
  int show_ai_status;
  int show_2d;
  int show_fps;
  int softwareRendering;
  int show_floor_texture;
  int line_spacing;
  int antialias_lines;
  int show_model; 
  int show_decals;
  int alpha_trails;
  int shadow_lod;
  int turn_cycle;
  int light_cycles;
  int lod;
  int fov;
  int stretch_textures;
  int show_skybox;
  int show_recognizer;
  int show_glow;
  int show_crash_texture;
  int show_wall;
  int fast_finish;
  int playMusic;
  int playEffects;
  int camType;
  float znear;
  float clear_color[4];
} SettingsCache;

typedef struct Game2 {
  Grid grid;
  RuleSet rules;
  SettingsCache settingsCache;
  DisplayList displayLists;
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
  
typedef struct Line {
  int sx, sy, ex, ey;
} Line;

typedef struct fontbmp {
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  texture *tex;
} fontbmp;

typedef struct fonttex {
  GLint nTextures;
  
  int texwidth; /* texture width */
  int width; /* character width */

  int lower; /* lowest ascii character (normally: 32) */
  int upper; /* highest ascii character (normally: 126) */

  GLuint *texID;

  char *fontname;
  char *bitmapname;
  int bitmapTexwidth;
  int bitmapWidth; /* character width */
} fonttex;

typedef struct Data {
  int iposx, iposy;
  float posx, posy;
  float t;
  
  int dir; int last_dir;

  unsigned int turn_time; /* for cycle animation */
  unsigned int spoke_time; /* for cycle wheel animation */
  int spoke_state; /* showing spoke or not */

  int score;
  float speed; /* set to -1 when dead */
  float trail_height; /* countdown to zero when dead */
  float exp_radius; /* explosion of the cycle model */
  Line *trails;
  Line *trail; /* current trail */
} Data;

enum { 
  CAM_FREE_R = 0,
  CAM_FREE_PHI,
  CAM_FREE_CHI 
};

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
  float movement[3]; /* indices CAM_R, CAM_CHI, CAM_PHI */
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
  /* char *name; currently ignored */
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
} gDisplay;

typedef struct Player {
  Data *data;
  gDisplay *display;
  Camera *camera;
  AI *ai;

  float pColorDiffuse[4];
  float pColorSpecular[4];
  float pColorAlpha[4];
} Player;

#define PLAYER_IS_ACTIVE(x) ((x)->data->speed > 0)

enum {
  BILINEAR = 0,
  TRILINEAR = 1
};

typedef enum ViewportType {
    VP_SINGLE = 0,
    VP_SPLIT = 1,
    VP_FOURWAY = 2
} ViewportType;

typedef struct Game {
  gDisplay *screen;
  ViewportType viewportType;
  Player *player;
  int players; /* number of players - currently limited to 4 somewhere */
  int winner; /* who won this round */
  int pauseflag; /* if the game is finished: the PAUSE_GAME_FINISHED flag
		    is set */
  int running; /* the amount of players that are still alive */
} Game;

#endif
