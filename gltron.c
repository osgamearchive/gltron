/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "gltron.h"

/* todo: define the globals where I need them */
/* declare them only in gltron.h */
/* #include "globals.h" */

void mouseWarp() {
  SystemWarpPointer(game->screen->w / 2, game->screen->h / 2);
}

void drawGame() {
  GLint i;
  gDisplay *d;
  Player *p;

  polycount = 0;

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glClearColor(.0, .0, .0, .0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for(i = 0; i < vp_max[ game->settings->display_type]; i++) {
    p = &(game->player[ game->settings->content[i] ]);
    if(p->display->onScreen == 1) {
      d = p->display;
      glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
      drawCam(p, d);
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      drawScore(p, d);
      if(game->settings->show_ai_status)
	if(p->ai->active == 1)
	  drawAI(d);
    }
  }

  if(game->settings->show_2d > 0) {
    drawDebugTex(game->screen);
    drawDebugLines(game->screen);
  }
  if(game->settings->show_fps)
    drawFPS(game->screen);

  drawConsole(game->screen);
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
  printf("OpenGL Info: '%s'\n%s - %s\n", glGetString(GL_VENDOR),
	 glGetString(GL_RENDERER), glGetString(GL_VERSION));
  printf("Extensions available: %s\n", glGetString(GL_EXTENSIONS));
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
  if(path != NULL)
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
  setFxVolume(game->settings->fxVolume);
  path = getFullPath("gltron.it");
  loadSound(path);
  if(game->settings->playMusic)
    playSound();
  setMusicVolume(game->settings->musicVolume);
  free(path);
#endif

  printf("loading menu\n");
  path = getFullPath("menu.txt");
  if(path != NULL)
    pMenuList = loadMenuFile(path);
  else {
    printf("fatal: could not load menu.txt, exiting...\n");
    exit(1);
  }
  printf("menu loaded\n");
  free(path);

  consoleInit();
  initGameStructures();
  resetScores();

  setupDisplay(game->screen);
  switchCallbacks(&guiCallbacks);
  switchCallbacks(&guiCallbacks);

  SystemMainLoop();

  return 0;
}

callbacks gameCallbacks = { 
  displayGame, idleGame, keyGame, initGame, exitGame, initGLGame, NULL, NULL
};











