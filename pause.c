#include "gltron.h"

/* very brief - just the pause mode */

void idlePause() {
#ifdef SOUND
  soundIdle();
#endif
  /* 
  if(SystemGetElapsedTime() - lasttime < 10) return;
  timediff();
  */

  SystemPostRedisplay();
}

void displayPause() {
  drawGame();
  drawPause(game->screen);

  if(game->settings->mouse_warp)
    mouseWarp();
  SystemSwapBuffers();
}

void keyboardPause(int key, int x, int y) {
  int i;
  switch(key) {
  case 27:
    switchCallbacks(&guiCallbacks);
    break;
  case ' ':
    if(game->pauseflag & PAUSE_GAME_FINISHED)
      initData();
    /* lasttime = SystemGetElapsedTime(); */
    switchCallbacks(&gameCallbacks);
    break;
  case 'q':
    exit(1);
    break;
  case SYSTEM_KEY_F1: defaultDisplay(0); break;
  case SYSTEM_KEY_F2: defaultDisplay(1); break;
  case SYSTEM_KEY_F3: defaultDisplay(2); break;
  case SYSTEM_KEY_F4: defaultDisplay(3); break;

  case SYSTEM_KEY_F10:
    game->settings->camType = (game->settings->camType + 1) % CAM_COUNT;
    for(i = 0; i < game->players; i++)
      game->player[i].camera->camType = game->settings->camType;
    break;
  case SYSTEM_KEY_F12: doScreenShot(); break;
    
  case SYSTEM_KEY_UP: consoleScrollBackward(1); break;
  case SYSTEM_KEY_DOWN: consoleScrollForward(1); break;

  case SYSTEM_KEY_F5: saveSettings(); break;
  case SYSTEM_KEY_TAB: switchCallbacks(&promptCallbacks); break;
  }
}

void initPause() {
  fprintf(stderr, "init pause mode\n");
}

void exitPause() {
}

void initPauseGL() {
  initGLGame();
}

callbacks pauseCallbacks = {
  displayPause, idlePause, keyboardPause,
  initPause, exitPause, initPauseGL, NULL, NULL
};

void keyboardPrompt(int key, int x, int y) {
  switch(key) {
  case 27:
  case SYSTEM_KEY_TAB:
    restoreCallbacks();
    break;
  case SYSTEM_RETURN:
    /* promptEvaluate(); */
    break;
  }
}

void initPrompt() { }
void exitPrompt() { }

callbacks promptCallbacks = {
  displayPause, idlePause, keyboardPrompt,
  initPrompt, exitPrompt, NULL, NULL, NULL
};




