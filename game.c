#include "gltron.h"

void initClientData() {
  /* for each player */
  /*   init camera (if any) */

  /* needs read access to server data
     to initialize camera */

  int i;
  int camType;
  Camera *cam;
  Data *data;

  for(i = 0; i < game->players; i++) {
    cam = game->player[i].camera;
    data = game->player[i].data;

    camType = (game->player[i].ai->active == AI_COMPUTER) ? 0 : game->settings->camType;
    initCamera(cam, data, camType);

    memcpy(game->player[i].pColorDiffuse, 
	   colors_model_diffuse[i], sizeof( float[4] ));
    memcpy(game->player[i].pColorSpecular, 
	   colors_model_specular[i], sizeof( float[4] ));
    memcpy(game->player[i].pColorAlpha, colors_alpha[i], sizeof( float[4] ));
#ifdef SOUND
    Audio_StartEngine(i);
#endif
  }
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

static void defaultViewportPositions() {
  game->settings->content[0] = 0;
  game->settings->content[1] = 1;
  game->settings->content[2] = 2;
  game->settings->content[3] = 3;
}


/*
  autoConfigureDisplay
      config viewports so every human player has one
 */
static void autoConfigureDisplay() {
  int n_humans = 0;
  int i;
  ViewportType vp;

  defaultViewportPositions();

  /* loop thru players and find the humans */
  for (i=0; i < game->players; i++) {
    if (game->player[i].ai->active == AI_HUMAN) {
      game->settings->content[n_humans] = i;
      n_humans++;
    }    
  }
 
  switch(n_humans) {
    case 0 :
      /*
         Not sure what the default should be for
         a game without human players. For now 
         just show a single viewport.
       */
      /* fall thru */
    case 1 :
      vp = VP_SINGLE;
      break;
    case 2 :
      vp = VP_SPLIT;
      break;
    default :
      defaultViewportPositions();
      vp = VP_FOURWAY;
  }  

  updateDisplay(vp);
}

void changeDisplay() {
  if (game->settings->display_type == 3) {
     autoConfigureDisplay(); 
  } else {
    defaultViewportPositions(); 
    updateDisplay(game->settings->display_type);
  }
}

void updateDisplay(ViewportType newVP) {
  int i;

  game->viewportType = newVP;

  for (i = 0; i < game->players; i++) {
    game->player[i].display->onScreen = 0;
  }
  for (i = 0; i < vp_max[newVP]; i++) {
       initDisplay(game->player[ game->settings->content[i] ].display, 
		   newVP, i, 1);
  }

}

void initGame() { /* called when game mode is entered */
  SystemHidePointer();
  SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
  game2->time.offset = SystemGetElapsedTime() - game2->time.current;
#ifdef SOUND
  Audio_EnableEngine();
#endif
}

void exitGame() {
  SystemUnhidePointer();
#ifdef SOUND
  Audio_DisableEngine();
#endif
}

void defaultDisplay(int n) {
  printf("set display to %d\n", n);
  game->settings->display_type = n;
  defaultViewportPositions();
  updateDisplay(n);
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
  if(x != MOUSE_ORIG_X || y != MOUSE_ORIG_Y) {
    game2->input.mousex += x - MOUSE_ORIG_X;
    game2->input.mousey += y - MOUSE_ORIG_Y;
    /* fprintf(stderr, "Mouse: dx: %d\tdy: %d\n", 
       x - MOUSE_ORIG_Y, y - MOUSE_ORIG_Y); */
    /* 
       cam_phi += - (x - MOUSE_ORIG_X) * MOUSE_CX;
       cam_chi += (y - MOUSE_ORIG_Y) * MOUSE_CY;
       if(cam_chi < CAM_CHI_MIN) cam_chi = CAM_CHI_MIN;
       if(cam_chi > CAM_CHI_MAX) cam_chi = CAM_CHI_MAX;
    */
    SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
  }
}

void gameMouse(int buttons, int state, int x, int y) {
  if(state == SYSTEM_MOUSEPRESSED) {
    if(buttons == SYSTEM_MOUSEBUTTON_LEFT) game2->input.mouse1 = 1;
    if(buttons == SYSTEM_MOUSEBUTTON_RIGHT) game2->input.mouse2 = 1;
  } else if(state == SYSTEM_MOUSERELEASED) {
    if(buttons == SYSTEM_MOUSEBUTTON_LEFT) game2->input.mouse1 = 0;
    if(buttons == SYSTEM_MOUSEBUTTON_RIGHT) game2->input.mouse2 = 0;
  }

  /*
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
  */
  /* fprintf(stderr, "new cam_r: %.2f\n", cam_r); */
}






