#include "gltron.h"
#include "event.h"

enum {
  TURN_LEFT = 3,
  TURN_RIGHT = 1
};

int getCol(int x, int y) {
  if(x < 0 || x >= game2->rules.grid_size -1 ||
     y < 0 || y >= game2->rules.grid_size -1 ||
     colmap[ y * colwidth + x ] != 0)
    return 1;
  else return 0;
}

void initGameStructures() { /* called only once */
  /* default art names */
  char *path;
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
  Player *p;

  game->winner = -1;
  game->screen = (gDisplay*) malloc(sizeof(gDisplay));
  game->viewportType = getSettingi("display_type"); 
  
  /* Setup the game's ai */
  initGameAI(getSettingi("ai_level"));
  
  d = game->screen;
  d->w = getSettingi("width"); 
  d->h = getSettingi("height"); 
  d->vp_x = 0; d->vp_y = 0;
  d->vp_w = d->w; d->vp_h = d->h;
  d->blending = 1;
  d->fog = 0;
  d->shademodel = GL_SMOOTH;
  d->wall = 1;
  d->onScreen = -1;
  d->textures = (unsigned int*) malloc(game_textures * sizeof(unsigned int));

  game->players = PLAYERS;
  game->player = (Player *) malloc(MAX_PLAYERS * sizeof(Player));
  for(i = 0; i < game->players; i++) {
    p = (game->player + i);
    p->display = (gDisplay*) malloc(sizeof(gDisplay));
    p->ai = (AI*) malloc(sizeof(AI));
    p->data = (Data*) malloc(sizeof(Data));
    p->data->trails = (line*) malloc(MAX_TRAIL * sizeof(line));
    p->camera = (Camera*) malloc(sizeof(Camera));
    p->camera->type = (CameraType*) malloc(sizeof(CameraType));

  }

  /* load recognizer model */
  path = getPath(PATH_DATA, "recognizer.obj.gz");
  if(path != NULL) {
    recognizer = readTriMeshFromFile(path);
    /* old code did normalize & invert normals & rescale to size = 60 */
  } else {
    fprintf(stderr, "fatal: could not load %s - exiting...\n", path);
    exit(1);
  }
  free(path);
 
  /* load recognizer  quad model (for recognizer outlines) */
  path = getPath(PATH_DATA, "recognizer_quad.obj.gz");
  if(path != NULL) {
    recognizer_quad = readQuadMeshFromFile(path);
    /* old code did normalize & invert normals & rescale to size = 60 */
  } else {
    fprintf(stderr, "fatal: could not load %s - exiting...\n", path);
    exit(1);
  }
  free(path);


  /* load lyghtcycle models */
  for(i = 0; i < LC_LOD; i++) {
    path = getPath(PATH_DATA, lc_lod_names[i]);
    if(path != NULL) {
      lightcycle[i] = readTriMeshFromFile(path);
    } else {
      fprintf(stderr, "fatal: could not load model %s - exiting...\n", lc_lod_names[i]);
      exit(1);
    }
  }
  free(path);

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
    float startpos[][2] = { 
	{ 0.5, 0.25 }, { 0.75, 0.5 }, { 0.5, 0.75 }, { 0.25, 0.5 }
    };
    /* float startdir[] = { 2, 1, 0, 3 }; */

    data = game->player[i].data;
    ai = game->player[i].ai;
    /* init ai */

    if(getSettingi("screenSaver")) {
      ai->active = AI_COMPUTER;
    } else {
      switch(i) {
      case 0: ai->active = getSettingi("ai_player1"); break;
      case 1: ai->active = getSettingi("ai_player2"); break;
      case 2: ai->active = getSettingi("ai_player3"); break;
      case 3: ai->active = getSettingi("ai_player4"); break;
      default:
	fprintf(stderr, "player index #%d not caught!\n", i);
	ai->active = AI_NONE;
      }
    }
    ai->tdiff = 0;
    ai->moves = getSettingi("grid_size") / 10;
    ai->danger = 0;
    ai->lastx = 0;
    ai->lasty = 0;

    /* arrange players in circle around center */

    data->iposx = startpos[i][0] * getSettingi("grid_size");
    data->iposy = startpos[i][1] * getSettingi("grid_size");
    if(i == 0) data->iposy -= 1;
    /* randomize starting direction */
    data->dir = rand() & 3;
    /* data->dir = startdir[i]; */
    data->last_dir = data->dir;
    data->posx = data->iposx;
    data->posy = data->iposy;
    data->t = 0;
    data->turn_time = -TURN_LENGTH;

    /* if player is playing... */
    if(ai->active != AI_NONE) {
      data->speed = getSettingf("speed");
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

  colwidth = getSettingi("grid_size");
  if(colmap != NULL)
    free(colmap);
  
  colmap = (unsigned char*) malloc(colwidth*colwidth/*getSettingi("grid_size")*/);
  for(i = 0; i < colwidth * colwidth/*getSettingi("grid_size")*/; i++)
    *(colmap + i) = 0;

  if(debugtex != NULL)
    free(debugtex);
  
  debugtex = (unsigned char*) malloc(4 * DEBUG_TEX_W * DEBUG_TEX_H);
  memset(debugtex, 0, 4 * DEBUG_TEX_W * DEBUG_TEX_H);
  ogl_debugtex = 0;

  /* lasttime = SystemGetElapsedTime(); */
  game->pauseflag = 0;

  game2->rules.speed = getSettingf("speed");
  game2->rules.eraseCrashed = getSettingi("erase_crashed");
  game2->rules.grid_size = getSettingi("grid_size");

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

#if 0
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
#endif

int updateTime() {
  game2->time.lastFrame = game2->time.current;
  game2->time.current = SystemGetElapsedTime() - game2->time.offset;
  game2->time.dt = game2->time.current - game2->time.lastFrame;
  /* fprintf(stderr, "dt: %d\n", game2->time.dt); */
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

  for(i = 0; i < colwidth *  game2->rules.grid_size; i++)
    if(colmap[i] == player + 1)
      colmap[i] = 0;

  /* This causes a substantial hiccup on OS X */
#if 0
  rebuildDebugTex();
#endif
}

void doCrashPlayer(GameEvent *e) {
  int j;

#ifdef SOUND
  Audio_CrashPlayer(e->player);
  Audio_StopEngine(e->player);
#endif

  for(j = 0; j < game->players; j++) 
    if(j != e->player && PLAYER_IS_ACTIVE(&(game->player[j])))
      game->player[j].data->score++;

  game->player[e->player].data->speed = SPEED_CRASHED;

  if(game2->rules.eraseCrashed == 1)
    clearTrail(e->player);
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
  tx = (float) x * DEBUG_TEX_W /  game2->rules.grid_size; 
  ty = (float) y * DEBUG_TEX_H /  game2->rules.grid_size;
  memcpy(debugtex + (int)ty * DEBUG_TEX_W * 4 + (int)tx * 4, source, 4);
}

void newTrail(Data* data) {
  line *newline;

  data->trail->ex = data->iposx;
  data->trail->ey = data->iposy;

  newline = data->trail + 1; /* new line */

  newline->sx = data->iposx;
  newline->sy = data->iposy;

  data->trail = newline;
}
      
static void doTurn(GameEvent *e, int direction) {
  Data *data = game->player[e->player].data;

  newTrail(data);
  data->last_dir = data->dir;
  data->dir = (data->dir + direction) % 4;
  data->turn_time = game2->time.current;
  data->posx = e->x + data->t * dirsX[data->dir];
  data->posy = e->y + data->t * dirsY[data->dir];
}

void doRightTurn(GameEvent *e) {
  doTurn(e, TURN_RIGHT);
}

void doLeftTurn(GameEvent *e) {
  doTurn(e, TURN_LEFT);
}
