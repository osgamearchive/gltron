#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "basic_types.h"
#include "settings.h"
#include "client_data.h"
#include "camera.h"

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
} RuleSet;


typedef struct Game2 {
  Grid grid;
  RuleSet rules;
  SettingsCache settingsCache;
  int mode;
  int players;
  int *startPositions;
  SystemTime time;
  List events;
  FILE *record;
  FILE *play;
  Input input;
} Game2;

typedef struct Data {
  // int iposx, iposy;
  float posx, posy;
	// float t;
  
  int dir; int last_dir;

  unsigned int turn_time; /* for cycle animation */
  unsigned int spoke_time; /* for cycle wheel animation */
  int spoke_state; /* showing spoke or not */

  int score;
  float speed; /* set to -1 when dead */
  float trail_height; /* countdown to zero when dead */

  /* explosion stuff */
  float impact_radius;
  float exp_radius; /* explosion of the cycle model */
 
  Line *trails;
	int trailOffset;
  // Line *trail; /* current trail */
	// int trailCount;
} Data;

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
  Visual *display;
  Camera *camera;
  AI *ai;

  float pColorDiffuse[4];
  float pColorSpecular[4];
  float pColorAlpha[4];
} Player;

typedef struct Game {
  Visual *screen;
  int viewportType;
  Player *player;
  int players; /* number of players - currently limited to 4 somewhere */
  int winner; /* who won this round */
  int pauseflag; /* if the game is finished: the PAUSE_GAME_FINISHED flag
		    is set */
  int running; /* the amount of players that are still alive */
} Game;

#endif
