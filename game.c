#include "gltron.h"

static int lod_n = 3;
static char *lod_names[] = {
  "lightcycle high.obj",
  "lightcycle med.obj",
  "lightcycle low.obj"
};
/* static int lod_dist[] = { 25, 50, 150 }; */

void initClientData() {
  /* for each player */
  /*   init camera (if any) */

  /* needs read access to server data
     to initialize camera */

  int i, j;
  Camera *cam;
  Data *data;
  Model *model;

  for(i = 0; i < game->players; i++) {
    cam = game->player[i].camera;
    data = game->player[i].data;
    model = game->player[i].model;

    for(j = 0; j < model->lod; j++)
      setMaterialAlphas(model->mesh[j], 1.0);

    cam->camType = game->settings->camType;
    cam->target[0] = data->posx;
    cam->target[1] = data->posy;
    cam->target[2] = 0;

    cam->cam[0] = data->posx + CAM_CIRCLE_DIST;
    cam->cam[1] = data->posy;
    cam->cam[2] = CAM_CIRCLE_Z;
  }

  cam_chi = M_PI / 3;
  cam_phi = 0;
}

void initDisplay(gDisplay *d, int type, int p, int onScreen) {
  int field;
  field = game->screen->vp_w / 32;
  d->h = game->screen->h;
  d->w = game->screen->w;
  d->vp_x = game->screen->vp_x + vp_x[type][p] * field;
  d->vp_y = game->screen->vp_y + vp_y[type][p] * field;
  d->vp_w = vp_w[type][p] * field;
  d->vp_h = vp_h[type][p] * field;
  d->blending = 1;
  d->fog = 0;
  d->wall = 1;
  d->onScreen = onScreen;
}  

void changeDisplay() {
  int i;
  for(i = 0; i < game->players; i++)
    game->player[i].display->onScreen = 0;
  for(i = 0; i < vp_max[game->settings->display_type]; i++)
       initDisplay(game->player[ game->settings->content[i] ].display, 
		   game->settings->display_type, i, 1);
}

void playEngine(void *data, Uint8 *stream, int len) {
  mixEngineSound(0, stream, len);
}

void initGame() { /* called when game mode is entered */
  game2->time.offset = SystemGetElapsedTime() - game2->time.current;
#ifdef SOUND
  Mix_SetPostMix(playEngine, NULL);
#endif
}

void exitGame() {
#ifdef SOUND
  Mix_SetPostMix(NULL, NULL);
#endif
}

void initModel(Player *p, int p_num) {
  int i, j;
  char *path;

  p->model->lod = lod_n;
  p->model->mesh = (Mesh**) malloc(lod_n * sizeof(Mesh*));
  /* p->model->lod_dist = (int*) malloc(lod_n * sizeof(int)); */

  for(i = 0; i < lod_n; i++) {
    /* p->model->lod_dist[i] = lod_dist[i]; */
    /* load player mesh, currently only one type */
    path = getFullPath(lod_names[i]);
    /* path = getFullPath("tron-med.obj"); */
    if(path != NULL)
      /* model size == CYCLE_HEIGHT */
      p->model->mesh[i] = loadModel(path, CYCLE_HEIGHT, 0);
    else {
      printf("fatal: could not load model - exiting...\n");
      exit(1);
    }
    
    free(path);
  }

  /* copy contents from colors_a[] to model struct */
  for(j = 0; j < 4; j++) {
    p->model->color_alpha[j] = colors_alpha[p_num][j];
    p->model->color_model[j] = colors_model[p_num][j];
  }
  /* set material 0 to color_model */
  for(i = 0; i < lod_n; i++) {
    setMaterialAmbient(p->model->mesh[i], 0, p->model->color_model);
    setMaterialDiffuse(p->model->mesh[i], 0, p->model->color_model);
  }
}

void defaultDisplay(int n) {
  printf("set display to %d\n", n);
  game->settings->display_type = n;
  game->settings->content[0] = 0;
  game->settings->content[1] = 1;
  game->settings->content[2] = 2;
  game->settings->content[3] = 3;
  changeDisplay();
}

void initGameScreen() {
  gDisplay *d;
  d = game->screen;
  d->h = game->settings->height; d->w = game->settings->width;
  d->vp_x = 0; d->vp_y = 0;
  d->vp_w = d->w; d->vp_h = d->h;
}

void cycleDisplay(int p) {
  int q;
  q = (game->settings->content[p] + 1) % game->players;
  while(q != game->settings->content[p]) {
    if(game->player[q].display->onScreen == 0)
      game->settings->content[p] = q;
    else q = (q + 1) % game->players;
  }
  changeDisplay();
}

void gameMouseMotion(int x, int y) {
  if(game->settings->camType == CAM_TYPE_MOUSE) 
    if(x != MOUSE_ORIG_X || y != MOUSE_ORIG_Y) {
      /* fprintf(stderr, "Mouse: dx: %d\tdy: %d\n", 
       x - MOUSE_ORIG_Y, y - MOUSE_ORIG_Y); */
      cam_phi += - (x - MOUSE_ORIG_X) * MOUSE_CX;
      cam_chi += (y - MOUSE_ORIG_Y) * MOUSE_CY;
      if(cam_chi < CAM_CHI_MIN) cam_chi = CAM_CHI_MIN;
      if(cam_chi > CAM_CHI_MAX) cam_chi = CAM_CHI_MAX;
      SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
    }
}

#define CAM_DR 1.0

void gameMouse(int buttons, int state, int x, int y) {
  if(game->settings->camType == CAM_TYPE_MOUSE) 
    if(state == SYSTEM_MOUSEPRESSED) {
      if(buttons == SYSTEM_MOUSEBUTTON_LEFT) {
	cam_r -= CAM_DR;
	if(cam_r < CAM_R_MIN) cam_r = CAM_R_MIN;
      } else if(buttons == SYSTEM_MOUSEBUTTON_RIGHT) {
	cam_r += CAM_DR;
	if(cam_r > CAM_R_MAX) cam_r = CAM_R_MAX;
      }
    }
  /* fprintf(stderr, "new cam_r: %.2f\n", cam_r); */
}
