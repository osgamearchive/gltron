#ifndef DATA_H
#define DATA_H

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
} Game2;
  
typedef struct line {
  int sx, sy, ex, ey;
} line;

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

typedef struct AI {
  int active;
  int tdiff; /*  */
  int moves;
  long lasttime;
  int danger;
  int lastx, lasty;
} AI;

typedef struct Player {
  Data *data;
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
typedef struct Settings {
  /* these settings affect the visuals and sound etc.
     and are client side only */
  /* these two are ignored in multiplayer mode */
  int fast_finish;

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
  Settings *settings;
  Player *player;
  int players; /* number of players - currently limited to 4 somewhere */
  int winner; /* who won this round */
  int pauseflag; /* if the game is finished: the PAUSE_GAME_FINISHED flag
		    is set */
  int running; /* the amount of players that are still alive */
} Game;

#endif



