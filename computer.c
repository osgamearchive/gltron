#include "gltron.h"

int freeway(Data *data, int dir) {
  int i;
  int wd = 20;

  for(i = 1; i < wd; i++)
    if(getCol((int)data->posx + dirsX[dir] * i, (int)data->posy + dirsY[dir] * i,
       colwidth, colmap)) break;
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
  { 10, 20 },
  { 8, 10 },
  { 5, 8 },
  { 2, 5 },
  { 0, 0 },
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
  300, 300, 200, 100
};

static float max_moves[] = {
  0.30, 0.40, 0.5, 0.5
};

static int spiral[] = {
  10, 10, 10, 10
};
	
void doComputer(Player *me, Data *him) {
  AI *ai;
  Data *data;
  int i, j, level, x, y, rdist, ldist;

  if(me->ai == NULL) {
    printf("This player has no AI data!\n");
    return;
  }
  
  data = me->data;
  ai = me->ai;
  ai->moves++;

  level = game->settings->ai_level;
  /* avoid to short turns */
  if(SystemGetElapsedTime() - ai->lasttime < turn_time[level])
    return;

  /* first, check if we are in danger */
  /* check the highest level first! */
  for(i = level; i >= 0; i++) {
    for(j = probe_dists[i][0]; j <= probe_dists[i][1]; j++) {
      setPos(data, &x, &y);
      getDistPoint(data->dir, j, &x, &y);
      if(getCol(x, y, colwidth, colmap)) {
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
      if(getCol(x, y, colwidth, colmap)) {
	ldist = i;
	break;
      } else { ldist = i; }
    }
    for(i = 1; i < MAX_PROBE_DIST; i++) {
      setPos(data, &x, &y);
      getDistPoint(RIGHT(data->dir), i, &x, &y);
      if(getCol(x, y, colwidth, colmap)) {
	rdist = i;
	break;
      } else { rdist = i; }
    }
    /* decide where to turn */
    if(ai->danger > rdist && ai->danger > ldist) {
      ai->danger--;
      return;
    } else if(rdist > ldist && ai->tdiff > -spiral[level] ) {
      turn(data, TURN_LEFT);
      ai->tdiff--;
    } else if(rdist < ldist && ai->tdiff < spiral[level] ) {
      turn(data, TURN_RIGHT);
      ai->tdiff++;
    } else {
      if(ai->tdiff > 0) { turn(data, TURN_LEFT); ai->tdiff--; }
      else { turn(data, TURN_RIGHT); ai->tdiff++; }
    }
    ai->danger = 0;
    ai->lasttime = SystemGetElapsedTime();
  }
}





