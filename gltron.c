/*
  gltron 0.50 beta
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "gltron.h"

/* todo: define the globals where I need them */
/* declare them only in gltron.h */

#include "globals.h"

void mouseWarp() {
  SystemWarpPointer(game->screen->w / 2, game->screen->h / 2);
}

void drawGame() {
  GLint i;
  gDisplay *d;
  Player *p;

  polycount = 0;
  glClearColor(.0, .0, .0, .0);
  glDepthMask(GL_TRUE);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);

  for(i = 0; i < vp_max[ game->settings->display_type]; i++) {
    p = &(game->player[ game->settings->content[i] ]);
    if(p->display->onScreen == 1) {
      d = p->display;
      glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
      drawCam(p, d);
      drawScore(p, d);
      if(game->settings->show_ai_status)
	if(p->ai->active == 1)
	  drawAI(d);
    }
  }

  if(game->settings->show_2d > 0)
    drawDebugTex(game->screen);
  if(game->settings->show_fps)
    drawFPS(game->screen);

  /*
  if(game->settings->show_help == 1)
    drawHelp(game->screen);
  */

  /* printf("%d polys\n", polycount); */
}
void displayGame() {
  drawGame();
  if(game->settings->mouse_warp)
    mouseWarp();
  SystemSwapBuffers();
}

void initCustomLights() {
  /* float col[] = { .77, .77, .77, 1.0 }; */
  float col[] = { .95, .95, .95, 1.0 };
  float dif[] =  { 0.4, 0.4, 0.4, 1};
  float amb[] = { 0.05, 0.05, 0.05, 1};

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
  glLightfv(GL_LIGHT0, GL_SPECULAR, col);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
}

void initGLGame() {
  printf("OpenGL Info: '%s'\n%s - %s\n", glGetString(GL_VENDOR),
	 glGetString(GL_RENDERER), glGetString(GL_VERSION));

  glShadeModel( game->screen->shademodel );


  if(game->settings->show_alpha) 
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glFogf(GL_FOG_START, 50.0);
  glFogf(GL_FOG_END, 100.0);
  glFogf(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY, 0.1);
  glDisable(GL_FOG);


  /* TODO(3): incorporate model stuff */
  /* initLightAndMaterial(); */
  initCustomLights();

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
}

int initWindow() {
  int win_id;
  int flags;
  unsigned char fullscreen = 0;
  /* char buf[20]; */

  SystemInitWindow(0, 0, game->settings->width, game->settings->height);
  flags = SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_DEPTH;
  if(game->settings->windowMode == 0)
    fullscreen = SYSTEM_FULLSCREEN;
  SystemInitDisplayMode(flags, fullscreen);

  win_id = SystemCreateWindow("gltron");
  if (win_id < 0) {
    printf("could not create window...exiting\n");
    exit(1);
  }
  return win_id;
}

void reshape(int x, int y) {
  if(x < game->settings->height || x < game->settings->width)
    initGameScreen();
  if(x > game->settings->width )
    game->screen->vp_x = (x - game->settings->width) / 2;
  if(y > game->settings->height )
    game->screen->vp_y = (y - game->settings->height) / 2;
  changeDisplay();
}

void shutdownDisplay(gDisplay *d) {
  deleteTextures();
  deleteFonts();
  SystemDestroyWindow(d->win_id);
  printf("window destroyed\n");
}

void setupDisplay(gDisplay *d) {
  printf("trying to create window\n");
  d->win_id = initWindow();
  printf("window created\n");
  /* printf("win_id is %d\n", d->win_id); */
  printf("loading fonts...\n");
  initFonts();
  printf("loading textures...\n");
  initTexture(game->screen);

  SystemReshapeFunc(reshape);
}

int main( int argc, char *argv[] ) {
  char *path;

#ifdef __FreeBSD__
  fpsetmask(0);
#endif

  SystemInit(&argc, argv);

  path = getFullPath("settings.txt");
  if(path != 0)
    initMainGameSettings(path); /* reads defaults from ~/.gltronrc */
  else {
    printf("fatal: could not settings.txt, exiting...\n");
    exit(1);
  }

  parse_args(argc, argv);

  /* sound */

#ifdef SOUND
  printf("initializing sound\n");
  initSound();
  setMusicVolume(game->settings->musicVolume);
  setFxVolume(game->settings->fxVolume);
  path = getFullPath("gltron.it");
  if(path == 0 || loadSound(path)) 
    printf("error trying to load sound\n");
  else {
    if(game->settings->playMusic)
      playSound();
    free(path);
  }
#endif

  printf("loading menu\n");
  path = getFullPath("menu.txt");
  if(path != 0)
    pMenuList = loadMenuFile(path);
  else {
    printf("fatal: could not load menu.txt, exiting...\n");
    exit(1);
  }
  printf("menu loaded\n");
  free(path);

  initGameStructures();
  resetScores();

  initData();

  setupDisplay(game->screen);
  switchCallbacks(&guiCallbacks);
  switchCallbacks(&guiCallbacks);

  SystemMainLoop();

  return 0;
}

callbacks gameCallbacks = { 
  displayGame, idleGame, keyGame, initGame, exitGame, initGLGame
};

