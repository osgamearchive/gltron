#ifndef DATA_H
#define DATA_H

/* data structures */

#include "model.h"

typedef struct callbacks {
  void (*display)(void);
  void (*idle)(void);
  void (*keyboard)(int, int, int);
  void (*init)(void);
  void (*exit)(void);
  void (*initGL)(void);
  void (*mouse)(int, int, int, int);
  void (*mouseMotion)(int, int);
} callbacks;

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
  EVENT_TURN = 1,
  EVENT_CRASH = 2,
  EVENT_STOP = 4
};

typedef struct GameEvent {
  int type; /* what */
  int player; /* who */
  int x; /* where */
  int y;
} GameEvent;

typedef struct History History;
struct History {
  int timestamp;
  GameEvent event;
  History *next;
};
  
typedef struct Game2 {
  Grid grid;
  RuleSet rules;
  int players;
  int *startPositions;
  Time time;
  History *history;
  History *current;
} Game2;
  
typedef struct line {
  int sx, sy, ex, ey;
} line;

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
  
  int turn;
  int dir; int last_dir;

  int turn_time; /* for cycle animation */
  
  int score;
  float speed; /* set to -1 when dead */
  float trail_height; /* countdown to zero when dead */
  float exp_radius; /* explosion of the cycle model */
  line *trails;
  line *trail; /* current trail */
} Data;

typedef struct Camera {
  float cam[3];
  float target[3];
  float angle;
  int camType;
} Camera;

typedef struct AI {
  int active;
  int tdiff; /*  */
  int moves;
  long lasttime;
  int danger;
} AI;

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

  unsigned int texFloor; 
  unsigned int texWall_1;
  unsigned int texWall_2;
  unsigned int texWall_3;
  unsigned int texWall_4;
  unsigned int texGui;
  unsigned int texLogo;
  unsigned int texCrash;
  unsigned int texTrail;
  unsigned int texTrailDecal;
  unsigned int texDebug;
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
   3) add pointer to initSettingsData() in settings.c
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

  int show_floor_texture;
  int show_glow;
  int show_ai_status;
  int show_model;
  int lod;
  int show_crash_texture;
  int model_backwards;
  int turn_cycle; /* smooth turning */
  int line_spacing; /* space between lines when the floor texture is
		       disabled */
  int use_mipmaps; /* enable / disable mipmapping */
  int mipmap_filter; /* bilinear / trilinear */

  int camType;
  int mouse_warp;

  int display_type; /* 0-2 -> 1, 2 or 4 displays on the screen */
  int content[4]; /* max. 4 individual viewports on the screen */
  int windowMode; /* 0: fullscreen, non-zero: window mode */

  int fov;
  int width;
  int height;

  int playMusic;
  int playEffects;

  float musicVolume;
  float fxVolume;

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
  float speed;
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

#endif
