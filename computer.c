#include "gltron.h"

int freeway(Data *data, int dir) {
  int i;
  int wd = 20;

  for(i = 1; i < wd; i++)
    if(getCol((int)data->posx + dirsX[dir] * i, 
	      (int)data->posy + dirsY[dir] * i)) 
      break;
  return i;
}

void getDistPoint(int dir, int d, int *x, int *y) {
  *x += dirsX[dir] * d;
  *y += dirsY[dir] * d;
}

void setPos(Data *data, int *x, int *y) {
  *x = (int)data->posx;
  *y = (int)data->posy;
}

#define MAX_PROBE_DIST 20

#define LEFT(x) ((x + 3) % 4)
#define RIGHT(x) ((x + 1) % 4)

static int probe_dists[][2] = {
  { 8, 10 },
  { 2, 5 },
};

/*
static int target_dir[][2] = {
  { 0, 1 },
  { 2, 1 },
  { 3, 0 },
  { 3, 2 }
};
*/

static int turn_time[] = {
  200, 100
};

static float max_moves[] = {
  .4, 0.5
};

static int spiral[] = {
  10, 10
};
	
void doComputer(int player, int target) {
  AI *ai;
  Data *data;
  Player *me, *him;
  int i, j, level, x, y, rdist = 0, ldist = 0;

  me = &(game->player[ player ]);
  him = &(game->player[ target ]);
  if(me->ai == NULL) {
    printf("This player has no AI data!\n");
    return;
  }
  data = me->data;
  ai = me->ai;
  level = game->settings->ai_level;

  ai->moves++;
  /* avoid to short turns */
  if(game2->time.current - ai->lasttime < turn_time[level])
    return;


  /* first, check if we are in danger */
  /* check the highest level first! */
  for(i = level; i >= 0; i++) {
    for(j = probe_dists[i][0]; j <= probe_dists[i][1]; j++) {
      setPos(data, &x, &y);
      getDistPoint(data->dir, j, &x, &y);
      if(getCol(x, y)) {
	ai->danger = j;
	break;
      }
    }
    if(ai->danger != 0) break;
  }

  if(ai->danger != 0 || ai->moves > max_moves[level] * game->settings->grid_size) {
    ai->moves = 0;

    /* figure out how far it's to either side */
    for(i = 1; i < MAX_PROBE_DIST; i++) {
      setPos(data, &x, &y);
      getDistPoint(LEFT(data->dir), i, &x, &y);
      if(getCol(x, y)) {
	ldist = i;
	break;
      } else { ldist = i; }
    }
    for(i = 1; i < MAX_PROBE_DIST; i++) {
      setPos(data, &x, &y);
      getDistPoint(RIGHT(data->dir), i, &x, &y);
      if(getCol(x, y)) {
	rdist = i;
	break;
      } else { rdist = i; }
    }
    /* decide where to turn */
    if(ai->danger > rdist && ai->danger > ldist) {
      ai->danger--;
      return;
    } else if(rdist > ldist && ai->tdiff > -spiral[level] ) {
      createTurnEvent(player, TURN_RIGHT);
      ai->tdiff--;
    } else if(rdist < ldist && ai->tdiff < spiral[level] ) {
      createTurnEvent(player, TURN_LEFT);
      ai->tdiff++;
    } else {
      if(ai->tdiff > 0) { 
	createTurnEvent(player, TURN_RIGHT);
	ai->tdiff--; }
      else { 
	createTurnEvent(player, TURN_LEFT);
	ai->tdiff++; }
    }
    ai->danger = 0;
    ai->lasttime = game2->time.current;
  }
}

/* new AI code */

#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_FRONT 4
#define DIR_BACK 8

void getXYdir(int dir, int *x, int *y) {
  int tx = 0, ty = 0;
  if(dir & DIR_FRONT) { tx = *x; ty = *y; }
  if(dir & DIR_BACK) { tx = -*x; ty = -*y; }
  if(dir & DIR_LEFT) { tx += -*y; ty += *x; }
  if(dir & DIR_RIGHT) { tx += *y; ty += -*x; }
  *x = tx; *y = ty;
}

int freeway2(Data* data, int xdir, int ydir, int c) {
  if(xdir == 0 && ydir == 0) {
    fprintf(stderr, "bug: xdir == ydir == 0\n");
    return 0;
  }

  while(getCol(data->iposx + xdir * c, 
	       data->iposy + ydir * c) == 0) c++;
  return c;
}

/*
  if FRONT < 0.92063 go straight
  else if 0.92063 >= REAR_RIGHT turn left
  else if LEFT < RIGHT turn right
  else turn left
*/

#define MIN_FREE 1
#define MIN_TURN 5
#define CRIT_F 0.92063

static int min_turn[] = { 1, 1, 15, 6 };

/* level 2 and better */

void doComputer2(int player, int target) {
  /* target is ignored */
  Data *data;
  Player *me;
  AI *ai;
  int level;
  int front, rear_right, left, right;
  int x, y;
  float critical;

  me = &(game->player[ player ]);
  if(me->ai == NULL) {
    printf("This player has no AI data!\n");
    return;
  }
  ai = me->ai;
  level = game->settings->ai_level;
  data = me->data;
  if(abs(data->iposx - ai->lastx) < min_turn[level] &&
     abs(data->iposy - ai->lasty) < min_turn[level]) {
    /* fprintf(stderr, "too early\n"); */
    return;
  }

  critical = (1 - CRIT_F) * game->settings->grid_size;
  x = dirsX[ data->dir ];
  y = dirsY[ data->dir ];
  getXYdir(DIR_FRONT, &x, &y);
  front = freeway2(data, x, y, MIN_FREE);

  x = dirsX[ data->dir ];
  y = dirsY[ data->dir ];
  getXYdir(DIR_BACK | DIR_RIGHT, &x, &y);
  rear_right = freeway2(data, x, y, MIN_FREE);

  x = dirsX[ data->dir ];
  y = dirsY[ data->dir ];
  getXYdir(DIR_RIGHT, &x, &y);
  right = freeway2(data, x, y, MIN_FREE);

  x = dirsX[ data->dir ];
  y = dirsY[ data->dir ];
  getXYdir(DIR_LEFT, &x, &y);
  left = freeway2(data, x, y, MIN_FREE);

  /*
    fprintf(stderr, "-------------------------\n");
    fprintf(stderr, "front space %d\n", front);
    fprintf(stderr, "rear_right space %d\n", rear_right);
    fprintf(stderr, "right space %d\n", right);
    fprintf(stderr, "left space %d\n", left);
  */

  if(front < critical && (left > front || right > front) ) {
    /*
    if(critical <= rear_right && left > critical) {
      // turn left
      createTurnEvent(player, TURN_LEFT);
      fprintf(stderr, "turning left\n");
      return;
    }
    */
    if(left < right) {
      // turn right
      createTurnEvent(player, TURN_RIGHT);
      // fprintf(stderr, "turning right\n");
      // return;
    } else {
      // turn left 
      createTurnEvent(player, TURN_LEFT);
      // fprintf(stderr, "turning left\n");
      // return;
    }
    ai->lastx = data->iposx;
    ai->lasty = data->iposy;
  }
  
}





