/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "gltron.h"
#include <assert.h>

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

  SystemInitWindow(0, 0, getSettingi("width"), getSettingi("height"));

  if(getSettingi("windowMode") == 0) {
    fullscreen = SYSTEM_FULLSCREEN;
  }

  flags = SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_DEPTH;
  if(getSettingi("bitdepth_32"))
    flags |= SYSTEM_32_BIT;

  if(getSettingi("use_stencil"))
    flags |= SYSTEM_STENCIL;

  SystemInitDisplayMode(flags, fullscreen);

  win_id = SystemCreateWindow("gltron");      

  if (win_id < 0) { 
    if( getSettingi("use_stencil") ) {
      flags &= ~SYSTEM_STENCIL;
      SystemInitDisplayMode(flags, fullscreen);
      win_id = SystemCreateWindow("gltron");      
      if(win_id >= 0) {
	setSettingi("use_stencil", 0);
	goto SKIP;
      }
    }
    printf("[fatal] could not create window...exiting\n");
    exit(1); /* OK: critical, no visual */
  }

 SKIP:

  if(getSettingi("windowMode") == 0 || getSettingi("mouse_warp") == 1) {
    SystemGrabInput();
  } else {
    SystemUngrabInput();
  }

  return win_id;
}

void reshape(int x, int y) {
  if(x < getSettingi("height") || x < getSettingi("width"))
    initGameScreen();
  if(x > getSettingi("width") )
    game->screen->vp_x = (x - getSettingi("width")) / 2;
  if(y > getSettingi("height") )
    game->screen->vp_y = (y - getSettingi("height")) / 2;
  changeDisplay(-1);
}

void shutdownDisplay(gDisplay *d) {
  deleteTextures(d);
  deleteFonts();
  SystemDestroyWindow(d->win_id);
  // printf("[video] window destroyed\n");
}

void setupDisplay(gDisplay *d) {
  // fprintf(stderr, "[video] trying to create window\n");
  d->win_id = initWindow();
  // fprintf(stderr, "[video] window created\n");
  initRenderer();
  // printRendererInfo();
  // printf("win_id is %d\n", d->win_id);
  // fprintf(stderr, "[status] loading art\n");
  loadArt();

  SystemReshapeFunc(reshape);
}



int main( int argc, char *argv[] ) {

#ifdef __FreeBSD__
  fpsetmask(0);
#endif

#if defined (macintosh)
  setupMacDirectories();
#endif

  SystemInit(&argc, argv);

#ifdef LOCAL_DATA
#if !defined(WIN32) && !defined(macintosh)
  goto_installpath(argv[0]);
#endif
#endif
  initDirectories();



  /* initialize lua */
  scripting_Init();
  /* load basic scripting services */
  { 
    char *path;
    path = getPath(PATH_SCRIPTS, "basics.lua");
    scripting_RunFile(path);
    free(path);

    path = getPath(PATH_SCRIPTS, "audio.lua");
    scripting_RunFile(path);
    free(path);

    path = getPath(PATH_SCRIPTS, "video.lua");
    scripting_RunFile(path);
    free(path);
    
    path = getPath(PATH_SCRIPTS, "joystick.lua");
    scripting_RunFile(path);
    free(path);
  }

  /* initialize some global variables */
  initMainGameSettings();

  /* load some more defaults from config file */
  initDefaultSettings();

  /* go for .gltronrc (or whatever is defined in RC_NAME) */
  {
    char *path;
    path = getPossiblePath(PATH_PREFERENCES, RC_NAME);
    if (path != NULL) {
      if (fileExists(path)) {
        printf("[status] loading settings from %s\n", path);
	      scripting_RunFile(path);
      } else {
	      printf("[error] cannot load %s from %s\n", RC_NAME, path);
      }
      free(path);
    }
    else {
      printf("[fatal] can't get valid pref path for %s\n", RC_NAME);
      exit(1); // something is seriously wrong
    }
  }

  /* parse any comandline switches overrinding the loaded settings */
  parse_args(argc, argv);

  /* sanity check some settings */
  checkSettings();
	
  /* intialize the settings cache, remember to do that everytime you
     change something */
  updateSettingsCache();

  /* initialize the rest of the game's datastructures */
  consoleInit();
  initGameStructures();
  resetScores();

  /* probe for artpacks & songs */
  initArtpacks();
#ifdef SOUND
  Sound_initTracks();
#endif

  // printf("[status] loading menu\n");
  { 
    char *path;
    path = getPath(PATH_SCRIPTS, "menu.lua");
    scripting_RunFile(path);
    free(path);

    path = getPath(PATH_SCRIPTS, "menu_functions.lua");
    scripting_RunFile(path);
    free(path);
  }
  // printf("[status] menu loaded\n");

  setupDisplay(game->screen);
#ifdef SOUND
  Sound_setup();
#endif

  /* switch callbacks twice to establish stack */
	{ 
		char *path;
		path = getPath(PATH_SCRIPTS, "main.lua");
		scripting_RunFile(path);
	}

  return 0;
}

callbacks gameCallbacks = { 
  displayGame, idleGame, keyGame, initGame, exitGame, initGLGame, gameMouse, gameMouseMotion, "game"
};








