#include "gltron.h"
#include "geom.h"

int getCol(int x, int y) {
  if(x < 0 || x >= game->settings->grid_size -1 ||
     y < 0 || y >= game->settings->grid_size -1 ||
     colmap[ y * colwidth + x ] != 0)
    return 1;
  else return 0;
}

void initGameStructures() { /* called only once */
  /* init game screen */
  /* init players. for each player: */
  /*   init model */
  /*   init display */
  /*   init ai */
  /*   create data */
  /*     create trails */
  /*   create camera */

  gDisplay *d;
  int i;
  /* int onScreen; */
  /* Data *data; */
  /* Camera *c; */
  /* Model *m; */
  AI *ai;
  Player *p;

  game->winner = -1;
  game->screen = (gDisplay*) malloc(sizeof(gDisplay));
  d = game->screen;
  d->h = game->settings->height; d->w = game->settings->width;
  d->vp_x = 0; d->vp_y = 0;
  d->vp_w = d->w; d->vp_h = d->h;
  d->blending = 1;
  d->fog = 0;
  d->shademodel = GL_SMOOTH;
  d->wall = 1;
  d->onScreen = -1;

  game->players = PLAYERS;
  game->player = (Player *) malloc(MAX_PLAYERS * sizeof(Player));
  for(i = 0; i < game->players; i++) {
    p = (game->player + i);
    p->model = (Model*) malloc(sizeof(Model));
    p->display = (gDisplay*) malloc(sizeof(gDisplay));
    p->ai = (AI*) malloc(sizeof(AI));
    p->data = (Data*) malloc(sizeof(Data));
    p->data->trails = (line*) malloc(MAX_TRAIL * sizeof(line));
    p->camera = (Camera*) malloc(sizeof(Camera));

    /* init model & display & ai */
    initModel(p, i);

    ai = p->ai;
    if(game->settings->screenSaver) {
      ai->active = AI_COMPUTER;
    } else {
      switch(i) {
      case 0: ai->active = game->settings->ai_player1; break;
      case 1: ai->active = game->settings->ai_player2; break;
      case 2: ai->active = game->settings->ai_player3; break;
      case 3: ai->active = game->settings->ai_player4; break;
      default:
	fprintf(stderr, "player index #%d not caught!\n", i);
	ai->active = AI_NONE;
      }
    }
    ai->tdiff = 0;
    ai->moves = 0;
    ai->danger = 0;
  }

  changeDisplay();
  initData();
}

void initData() {
  /* for each player */
  /*   init camera (if any) */
  /*   init data */
  /*   reset ai (if any) */
  int i;
  Data *data;
  AI *ai;
  int not_playing = 0;

  for(i = 0; i < game->players; i++) {
    data = game->player[i].data;
    ai = game->player[i].ai;

    /* arrange players in circle around center */
    data->iposx = game->settings->grid_size / 2 +
      game->settings->grid_size / 4 *
      (float) cos ( (float) (i * 2 * M_PI) / (float) game->players );

    data->iposy = game->settings->grid_size / 2 +
      game->settings->grid_size / 4 * 
      (float) sin ( (float) (i * 2 * M_PI) / (float) game->players );

    data->posx = data->iposx;
    data->posy = data->iposy;
    data->t = 0;

    /* randomize starting direction */
    data->dir = rand() & 3;
    data->last_dir = data->dir;
    data->turn_time = 0;
    data->turn = 0;

    /* if player is playing... */
    if(ai->active != 2) {
      data->speed = game->settings->current_speed;
      data->trail_height = TRAIL_HEIGHT;
      data->exp_radius = 0;
    } else {
      data->speed = SPEED_GONE;
      data->trail_height = 0;
      data->exp_radius = EXP_RADIUS_MAX;

      not_playing++;
    }
    data->trail = data->trails;

    data->trail->sx = data->trail->ex = data->iposx;
    data->trail->sy = data->trail->ey = data->iposy;

    ai->tdiff = 0;
    ai->moves = game->settings->grid_size / 10;
    ai->danger = 0;
    ai->lasttime = SystemGetElapsedTime();
  }

  game->running = game->players - not_playing; /* not everyone is alive */
  /* printf("starting game with %d players\n", game->running); */
  game->winner = -1;
  /* colmap */

  /* TODO: check if grid_size/colwidth has changed and  
   *       reallocate colmap accordingly                */

  colwidth = game->settings->grid_size;
  if(colmap != NULL)
    free(colmap);
  colmap = (unsigned char*) malloc(colwidth * game->settings->grid_size);
  for(i = 0; i < colwidth * game->settings->grid_size; i++)
    *(colmap + i) = 0;

  if(debugtex != NULL)
    free(debugtex);
  
  debugtex = (unsigned char*) malloc(4 * DEBUG_TEX_W * DEBUG_TEX_H);
  memset(debugtex, 0, 4 * DEBUG_TEX_W * DEBUG_TEX_H);
  ogl_debugtex = 0;

  initClientData();
  lasttime = SystemGetElapsedTime();
  game->pauseflag = 0;
}

void idleGame( void ) {
  int i, j;
  int loop; 

#ifdef SOUND
  soundIdle();
#endif

  if(game->settings->fast_finish == 1) {
    loop = FAST_FINISH;
    for(i = 0; i < game->players; i++)
      if(game->player[i].ai->active == AI_HUMAN &&
	 game->player[i].data->exp_radius < EXP_RADIUS_MAX)
	 /* game->player[i].data->speed > 0) */
	loop = 1;
  } else loop = 1;

  if(SystemGetElapsedTime() - lasttime < 10 && loop == 1) return;
  timediff();
  for(j = 0; j < loop; j++) {
    if(loop == FAST_FINISH)
      dt = 20;
    movePlayers();

    /* do AI */
    for(i = 0; i < game->players; i++)
      if(game->player[i].ai != NULL)
	if(game->player[i].ai->active == 1)
	  doComputer(&(game->player[i]), game->player[i].data);
  }

  /* chase-cam movement here */
  camMove();
  chaseCamMove();

  SystemPostRedisplay();
}

void resetScores() {
  int i;
  for(i = 0; i < game->players; i++)
    game->player[i].data->score = 0;
}

void moveStep(Data* data) {
  data->iposx += dirsX[data->dir];
  data->iposy += dirsY[data->dir];
}

void clearTrail(int player) {
  int i;

  for(i = 0; i < colwidth * game->settings->grid_size; i++)
    if(colmap[i] == player + 1)
      colmap[i] = 0;
}

void crashPlayer(int player) {
  int j;

  if(game->settings->playEffects)
    playGameFX(fx_crash);

  for(j = 0; j < game->players; j++) 
    if(j != player && game->player[j].data->speed > 0)
      game->player[j].data->score++;

  game->player[player].data->speed = SPEED_CRASHED;

  if(game->settings->erase_crashed == 1)
    clearTrail(player);
}

void writePosition(int player) {
  unsigned char *source;
  int x, y;
  float tx, ty;

  x = game->player[player].data->iposx;
  y = game->player[player].data->iposy;

  colmap[ y * colwidth + x ] = player + 1;

  source = debugcolors[ colmap [ y * colwidth + x ] ];
  tx = (float) x * DEBUG_TEX_W / game->settings->grid_size;
  ty = (float) y * DEBUG_TEX_H / game->settings->grid_size;
  memcpy(debugtex + (int)ty * DEBUG_TEX_W * 4 + (int)tx * 4, source, 4);
}

void newTrail(Data* data) {
  line *new;

  data->trail->ex = data->iposx;
  data->trail->ey = data->iposy;

  new = data->trail + 1; /* new line */

  new->sx = data->iposx;
  new->sy = data->iposy;

  data->trail = new;
}
  
void movePlayers() {
  int i;
  float fs;
  Data *data;

  for(i = 0; i < game->players; i++) {
    data = game->player[i].data;
    if(data->speed > 0) { /* still alive */

#define FREQ 1200
#define FACTOR 0.09
      fs = 1.0 - FACTOR + 
	FACTOR * cos(i * M_PI / 4.0 + (float)(lasttime % FREQ) * 2.0 * M_PI / (float)FREQ);
#undef FREQ
#undef FACTOR

      data->t += dt / 100.0 * data->speed * fs;
      while(data->t >= 1) {
	moveStep(data);
	data->t--;
	if(getCol(data->iposx, data->iposy)) {
	  crashPlayer(i);
	  break;
	} else {
	  writePosition(i);
	}
      }
      if(data->turn) {
	newTrail(data);
	data->last_dir = data->dir;
	data->dir = (data->dir + data->turn) % 4;
	data->turn = 0;
      }
      data->posx = data->iposx + data->t * dirsX[data->dir];
      data->posy = data->iposy + data->t * dirsY[data->dir];
    } else { /* already crashed */
      if(game->settings->erase_crashed == 1 && data->trail_height > 0)
	data->trail_height -= (float)(dt * TRAIL_HEIGHT) / 1000;

      if(data->exp_radius < EXP_RADIUS_MAX)
	data->exp_radius += (float)dt * EXP_RADIUS_DELTA;
      else if (data->speed == SPEED_CRASHED) {
	int winner;

	data->speed = SPEED_GONE;
	game->running--;
	if(game->running <= 1) { /* all dead, find survivor */
	  for(winner = 0; winner < game->players; winner++)
	    if(game->player[winner].data->speed > 0) break;
	  game->winner = (winner == game->players) ? -1 : winner;
	  printf("winner: %d\n", winner);
	  switchCallbacks(&pauseCallbacks);
	  /* screenSaverCheck(0); */
	  game->pauseflag = PAUSE_GAME_FINISHED;
	}
      }
    }      
  }
}

void timediff() {
  int t;
  t = SystemGetElapsedTime();
  dt = t - lasttime;
  lasttime = t;
}


static float cam_park_pos[4][2] = {
  { 0.2, 0.2 },
  { 0.8, 0.2 },
  { 0.2, 0.8 },
  { 0.8, 0.8 }
};

static float park_height = 80.0;

void chaseCamMove() {
  int i;
  Camera *cam;
  Data *data;
  float dest[3];
  float tdest[3];
  float dcamx;
  float dcamy;
  float d;

  for(i = 0; i < game->players; i++) {
      
    cam = game->player[i].camera;
    data = game->player[i].data;
    
    if(data->speed == SPEED_GONE) {
      float tmp[3];
    
      dest[0] = cam_park_pos[i][0] * game->settings->grid_size;
      dest[1] = cam_park_pos[i][1] * game->settings->grid_size;
    
      dest[2] = park_height;
      tdest[0] = game->settings->grid_size / 2;
      tdest[1] = game->settings->grid_size / 2;
      tdest[2] = -10;
      vsub(dest, cam->cam, tmp);
      if(length(tmp) > 1) {
	normalize(tmp);
	vadd(cam->cam, tmp, cam->cam);
      } else
	vcopy(dest, cam->cam);
      vsub(tdest, cam->target, tmp);
      if(length(tmp) > 1) {
	normalize(tmp);
	vadd(cam->target, tmp, cam->target);
      } else 
	vcopy(tdest, cam->target);
    } else {
      
      switch(cam->camType) {
      case 0: /* Andi-cam */
	cam->cam[0] = data->posx + CAM_CIRCLE_DIST * COS(camAngle);
	cam->cam[1] = data->posy + CAM_CIRCLE_DIST * SIN(camAngle);
	cam->cam[2] = CAM_CIRCLE_Z;
	cam->target[0] = data->posx;
	cam->target[1] = data->posy;
	cam->target[2] = B_HEIGHT;
	break;
      case 1: /* Mike-cam */
	cam->target[0] = data->posx;
	cam->target[1] = data->posy;
	cam->target[2] = B_HEIGHT;
      
	dest[0] = cam->target[0] - CAM_FOLLOW_DIST * dirsX[ data->dir];
	dest[1] = cam->target[1] - CAM_FOLLOW_DIST * dirsY[ data->dir];

	d = sqrt((dest[0] - cam->cam[0]) * (dest[0] - cam->cam[0]) +
		 (dest[1] - cam->cam[1]) * (dest[1] - cam->cam[1]));
	if(d != 0) {
	  /*
	    dcamx = (float)dt * CAM_FOLLOW_SPEED * (dest[0] - cam->cam[0]) / d;
	    dcamy = (float)dt * CAM_FOLLOW_SPEED * (dest[1] - cam->cam[1]) / d;
	  */
	  dcamx = (float)dt * CAM_FOLLOW_SPEED_FACTOR *
	    game->settings->current_speed * (dest[0] - cam->cam[0]) / d;
	  dcamy = (float)dt * CAM_FOLLOW_SPEED_FACTOR *
	    game->settings->current_speed * (dest[1] - cam->cam[1]) / d;

	  if((dest[0] - cam->cam[0] > 0 && dest[0] - cam->cam[0] < dcamx) ||
	     (dest[0] - cam->cam[0] < 0 && dest[0] - cam->cam[0] > dcamx)) {
	    cam->cam[0] = dest[0];
	  } else cam->cam[0] += dcamx;

	  if((dest[1] - cam->cam[1] > 0 && dest[1] - cam->cam[1] < dcamy) ||
	     (dest[1] - cam->cam[1] < 0 && dest[1] - cam->cam[1] > dcamy)) {
	    cam->cam[1] = dest[1];
	  } else cam->cam[1] += dcamy;
	}
	break;
      case 2: /* 1st person */
#define H 3
	cam->target[0] = data->posx + dirsX[data->dir];
	cam->target[1] = data->posy + dirsY[data->dir];
	cam->target[2] = H;

	cam->cam[0] = data->posx;
	cam->cam[1] = data->posy;
	cam->cam[2] = H + 0.5;
#undef H
	break;
      }
    }
  }
}

void camMove() {
  camAngle += CAM_SPEED * dt / 100;
  while(camAngle > 360) camAngle -= 360;
}
