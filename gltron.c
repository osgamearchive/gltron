/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "gltron.h"

/* todo: define the globals where I need them */
/* declare them only in gltron.h */
/* #include "globals.h" */

void displayGame() {
  drawGame();
  SystemSwapBuffers();
}

int initWindow() {
  int win_id;
  int flags;
  unsigned char fullscreen = 0;
  /* char buf[20]; */

  SystemInitWindow(0, 0, game->settings->width, game->settings->height);

  if(game->settings->windowMode == 0) {
    fullscreen = SYSTEM_FULLSCREEN;
  }

  flags = SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_DEPTH;
  if(game->settings->bitdepth_32)
    flags |= SYSTEM_32_BIT;

  SystemInitDisplayMode(flags, fullscreen);

  win_id = SystemCreateWindow("gltron");

  if (win_id < 0) {
    printf("could not create window...exiting\n");
    exit(1);
  }

  if(game->settings->windowMode == 0 || game->settings->mouse_warp == 1) {
    SystemGrabInput();
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
  deleteTextures(d);
  deleteFonts();
#if 0
  deleteBitmaps(d);
#endif
  SystemDestroyWindow(d->win_id);
  printf("window destroyed\n");
}

void setupDisplay(gDisplay *d) {
  fprintf(stderr, "trying to create window\n");
  d->win_id = initWindow();
  fprintf(stderr, "window created\n");
  initRenderer();
  printRendererInfo();
  /* printf("win_id is %d\n", d->win_id); */
  fprintf(stderr, "loading art\n");
  loadArt();

#if 0
  initBitmaps(game->screen);
#endif

  SystemReshapeFunc(reshape);
}



int main( int argc, char *argv[] ) {
  char *path;
  list *l;
#ifdef SOUND
  int c;
#endif

#ifdef __FreeBSD__
  fpsetmask(0);
#endif

#if defined (macintosh)
    setupHomeEnvironment ();
#endif

  SystemInit(&argc, argv);

#ifndef CURRENT_EQ_DATA_DIR
  goto_installpath(argv[0]);
#endif

  /* initialize artpack list before loading settigns! */
  initArtpacks();

  path = getFullPath("settings.txt");
  if(path != NULL)
    initMainGameSettings(path); /* reads defaults from ~/.gltronrc */
  else {
    printf("fatal: could not settings.txt, exiting...\n");
    exit(1);
  }

  parse_args(argc, argv);

  consoleInit();
  initGameStructures();
  resetScores();

  /* sound */
  path = getMusicPath(MUSIC_DIR);
  game->settings->soundList = 
    readDirectoryContents(path, SONG_PREFIX);
  
  game->settings->soundIndex = -1;

  l = game->settings->soundList;

#ifdef SOUND
  printf("initializing sound\n");
  initSound();
  setFxVolume(game->settings->fxVolume);

  if(l->next != NULL) {
    char *tmp;
    tmp = (char*)malloc(strlen(path) + 1 + /* seperator */
		 strlen((char*) l->data) + 1);
    sprintf(tmp, "%s%c%s", path, SEPERATOR, 
	    (char*) l->data);
    fprintf(stderr, "loading song %s\n", tmp);
    loadSound(tmp);
    free(tmp);
    game->settings->soundIndex = 0;
  }

  c = 0;
  while(l->next != NULL) {
    l = l->next;
    c++;
  }
  game->settings->soundSongCount = c;

  if(game->settings->playMusic)
    playSound();
  fprintf(stderr, "setting music volume to %.3f\n",
	  game->settings->musicVolume);
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

  setupDisplay(game->screen);
  switchCallbacks(&guiCallbacks);
  switchCallbacks(&guiCallbacks);

  SystemMainLoop();

  return 0;
}

callbacks gameCallbacks = { 
  displayGame, idleGame, keyGame, initGame, exitGame, initGLGame, gameMouse, gameMouseMotion
};

