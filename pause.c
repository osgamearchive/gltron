#include "gltron.h"

/* very brief - just the pause mode */

void idlePause() {
#ifdef SOUND
  soundIdle();
#endif
  doCameraMovement();
  if(getSettingi("screenSaver") && stoptime != 0 &&
     SystemGetElapsedTime() - stoptime > 2000) {
    initData();
    stoptime = 0;
    switchCallbacks(&gameCallbacks);
  }

  scripting_RunGC();
  SystemPostRedisplay();
}

void displayPause() {
  drawGame();
  drawPause(game->screen);

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
    /* case 'q': SystemExit(); break; */
  case SYSTEM_KEY_F1: defaultDisplay(0); break;
  case SYSTEM_KEY_F2: defaultDisplay(1); break;
  case SYSTEM_KEY_F3: defaultDisplay(2); break;
  case SYSTEM_KEY_F4:
#warning "why is this not calling defaultDisplay() ?"
    setSettingi("display_type", 3);
    changeDisplay();
    break;
    /* doesn't really work on my box */
    /*
  case SYSTEM_KEY_F6: 
    Gamma -= 0.1;
    shutdownDisplay(game->screen);
    SystemSetGamma(Gamma, Gamma, Gamma);
    setupDisplay(game->screen);
    break;
  case SYSTEM_KEY_F7: 
    Gamma += 0.1;
    shutdownDisplay(game->screen);
    SystemSetGamma(Gamma, Gamma, Gamma);
    setupDisplay(game->screen);
    break;
    */
  case SYSTEM_KEY_F10:
    setSettingi("camType", (getSettingi("camType") + 1) % CAM_COUNT);
    for(i = 0; i < game->players; i++)
      if(game->player[i].ai->active == AI_HUMAN)
	initCamera(game->player[i].camera, 
		   game->player[i].data, 
		   getSettingi("camType"));
    break;
  case SYSTEM_KEY_F11: doBmpScreenShot(game->screen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(game->screen); break;
    
  case SYSTEM_KEY_UP: consoleScrollBackward(1); break;
  case SYSTEM_KEY_DOWN: consoleScrollForward(1); break;

  case SYSTEM_KEY_F5: saveSettings(); break;
  case SYSTEM_KEY_TAB: switchCallbacks(&promptCallbacks); break;
  }
}

void initPause() {
  SystemHidePointer();
  SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
  fprintf(stderr, "init pause mode\n");
}

void exitPause() {
  SystemUnhidePointer();
}

void initPauseGL() {
  initGLGame();
}

callbacks pauseCallbacks = {
  displayPause, idlePause, keyboardPause,
  initPause, exitPause, initPauseGL, gameMouse, gameMouseMotion
};

void keyboardPrompt(int key, int x, int y) {
  switch(key) {
  case 27:
  case SYSTEM_KEY_TAB:
    restoreCallbacks();
    break;
  case SYSTEM_KEY_RETURN:
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




