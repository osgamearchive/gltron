#include "game/gltron.h"

/* very brief - just the pause mode */

void idlePause(void) {
  Sound_idle();
  game2->time.dt = 0;
  doCameraMovement();
	
  scripting_RunGC();
  SystemPostRedisplay();
}

void displayPause(void) {
  drawGame();
  drawPause(gScreen);

  SystemSwapBuffers();
}

void keyboardPause(int state, int key, int x, int y) {
	if(state == SYSTEM_KEYSTATE_UP)
		return;

  switch(key) {
  case 27:
		SystemExitLoop(RETURN_PAUSE_ESCAPE);
    break;
  case SYSTEM_KEY_F1: changeDisplay(0); break;
  case SYSTEM_KEY_F2: changeDisplay(1); break;
  case SYSTEM_KEY_F3: changeDisplay(2); break;
  case SYSTEM_KEY_F4: changeDisplay(3); break;

  case SYSTEM_KEY_F5: saveSettings(); break;

  case SYSTEM_KEY_F10: nextCameraType(); break;

  case SYSTEM_KEY_F11: doBmpScreenShot(gScreen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(gScreen); break;
    
  case SYSTEM_KEY_UP: consoleScrollBackward(1); break;
  case SYSTEM_KEY_DOWN: consoleScrollForward(1); break;

  case SYSTEM_KEY_TAB: 
	// SystemExitLoop(RETURN_MENU_PROMPT);
		break;

	default:
    if(game->pauseflag == PAUSE_GAME_FINISHED)
      initData();
    /* lasttime = SystemGetElapsedTime(); */
    SystemExitLoop(RETURN_GAME_UNPAUSE);
    break;
  }
}

void initPause(void) {
  SystemHidePointer();
  SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);

  /* disable game sound effects */
  Audio_DisableEngine();

  /* 
   * TODO: Provide an option to disable game music here. 
   * Game should be totally silent in pause mode. (Nice when 
   * the boss is walking by, phone call, etc...)
   */

  /* set pause flag to suspended */
  if (game->pauseflag != PAUSE_GAME_FINISHED) {
    game->pauseflag = PAUSE_GAME_SUSPENDED;
  }
  
  updateSettingsCache();
}

void exitPause(void) {
/* sound effects are re-enabled in enterGame() */
// Audio_EnableEngine(); 
}

void initPauseGL(void) {
  initGLGame();
}

Callbacks pauseCallbacks = {
  displayPause, idlePause, keyboardPause,
  initPause, exitPause, initPauseGL, gameMouse, gameMouseMotion, "pause"
};

void keyboardPrompt(int state, int key, int x, int y) {
	if(state == SYSTEM_KEYSTATE_UP)
		return;

  switch(key) {
  case 27:
  case SYSTEM_KEY_TAB:
		SystemExitLoop(RETURN_PAUSE_ESCAPE);
    break;
  case SYSTEM_KEY_RETURN:
    /* promptEvaluate(); */
    break;
  }
}

void initPrompt(void) { }
void exitPrompt(void) { }

Callbacks promptCallbacks = {
  displayPause, idlePause, keyboardPrompt,
  initPrompt, exitPrompt, NULL, NULL, NULL, "prompt"
};




