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
  deleteTextures(d);
  deleteFonts();
  deleteBitmaps(d);
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
  initBitmaps(game->screen);
  SystemReshapeFunc(reshape);
}

int main( int argc, char *argv[] ) {
  char *path;
  list *l;
  int c;

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
  path = getFullPath(MUSIC_DIR);

  game->settings->soundList = 
    readDirectoryContents(path, SONG_PREFIX);
  game->settings->soundIndex = -1;

  l = game->settings->soundList;
  c = 0;
  if(l->next != NULL) {
    char *tmp;
    tmp = malloc(strlen(path) + 1 + /* seperator */
		 strlen((char*) l->data) + 1);
    sprintf(tmp, "%s%c%s", path, SEPERATOR, 
	    (char*) l->data);
    fprintf(stderr, "loading song %s\n", tmp);
    loadSound(tmp);
    free(tmp);
    game->settings->soundIndex = 0;
  }
  while(l->next != NULL) {
    l = l->next;
    c++;
  }
  game->settings->soundSongCount = c;

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
