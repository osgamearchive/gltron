#include "gltron.h"

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

  game2->events.next = NULL;
  game2->mode = GAME_SINGLE;
}

void initPlayerData() {
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
    data->turn = 0;
    data->turn_time = -TURN_LENGTH;

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
    ai->lasttime = 0;
  }
  game->running = game->players - not_playing; /* not everyone is alive */
  /* printf("starting game with %d players\n", game->running); */
  game->winner = -1;
}

void initData() {
  int i;

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

  /* lasttime = SystemGetElapsedTime(); */
  game->pauseflag = 0;

  game2->rules.speed = game->settings->current_speed;
  game2->rules.eraseCrashed = game->settings->erase_crashed;
  /* time management */
  game2->time.lastFrame = 0;
  game2->time.current = 0;
  game2->time.offset = SystemGetElapsedTime();
  /* TODO: fix that */
  game2->players = game->players;
  /* event management */
  game2->events.next = NULL;
  /* TODO: free any old events that might have gotten left */

  initPlayerData();
  initClientData();
}

int applyGameInfo() {
  int i; 
  Data *data;

  if(game2->players > game->players) {
    fprintf(stderr, "more players in demo than allowed\n");
    return 1;
  }

  for(i = 0; i < game2->players; i++) {
    data = game->player[i].data;
    data->speed = game2->rules.speed;
    data->iposx = game2->startPositions[3 * i + 0];
    data->iposy = game2->startPositions[3 * i + 1];
    data->posx = data->iposx;
    data->posy = data->iposy;
    data->t = 0;
    data->dir = game2->startPositions[3 * i + 2];
    data->last_dir = data->dir;
  }

  for(; i < game->players; i++) {
    data = game->player[i].data;
    data->speed = SPEED_GONE;
    data->trail_height = 0;
    data->exp_radius = EXP_RADIUS_MAX;
  }
  return 0;
}

int updateTime() {
  game2->time.lastFrame = game2->time.current;
  game2->time.current = SystemGetElapsedTime() - game2->time.offset;
  game2->time.dt = game2->time.current - game2->time.lastFrame;
  return game2->time.dt;
}

void addList(list **l, void* data) {
  list *p;
  if(*l == NULL) {
    *l = (list*) malloc(sizeof(list));
    (*l)->next = NULL;
  }
  for(p = *l; p->next != NULL; p = p->next);
  p->next = (list*) malloc(sizeof(list));
  p->next->next = NULL;
  p->data = data;
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

  rebuildDebugTex();
}

void crashPlayer(int player) {
  int j;

#ifdef SOUND
  if(game->settings->playEffects)
    playGameFX(fx_crash);
#endif

  for(j = 0; j < game->players; j++) 
    if(j != player && game->player[j].data->speed > 0)
      game->player[j].data->score++;

  game->player[player].data->speed = SPEED_CRASHED;

  if(game2->rules.eraseCrashed == 1)
    clearTrail(player);
}

void writePosition(int player) {
  unsigned char *source;
  int x, y;
  float tx, ty;

  x = game->player[player].data->iposx;
  y = game->player[player].data->iposy;

  /* collision detection */
  colmap[ y * colwidth + x ] = player + 1;

  /* debug texture */
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
      
void doTurn(Data *data, int time) {
  newTrail(data);
  data->last_dir = data->dir;
  data->dir = (data->dir + data->turn) % 4;
  data->turn = 0;
  data->turn_time = game2->time.current;
  data->posx = data->iposx + data->t * dirsX[data->dir];
  data->posy = data->iposy + data->t * dirsY[data->dir];
}
