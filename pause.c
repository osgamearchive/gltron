#include "gltron.h"

/* very brief - just the pause mode */

void idlePause() {
#ifdef SOUND
  Sound_idle();
#endif
  game2->time.dt = 0;
  doCameraMovement();
	
	/*
  // if(getSettingi("screenSaver") && stoptime != 0 &&
	// SystemGetElapsedTime() - stoptime > 2000) {
	// initData();
	// stoptime = 0;
	// switchCallbacks(&gameCallbacks);
	// }
	*/


  scripting_RunGC();
  SystemPostRedisplay();
}

void displayPause() {
  drawGame();
  drawPause(game->screen);

  SystemSwapBuffers();
}

void keyboardPause(int key, int x, int y) {
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

  case SYSTEM_KEY_F11: doBmpScreenShot(game->screen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(game->screen); break;
    
  case SYSTEM_KEY_UP: consoleScrollBackward(1); break;
  case SYSTEM_KEY_DOWN: consoleScrollForward(1); break;

  case SYSTEM_KEY_TAB: 
	// SystemExitLoop(RETURN_MENU_PROMPT);
		break;

	default:
    if(game->pauseflag & PAUSE_GAME_FINISHED)
      initData();
    /* lasttime = SystemGetElapsedTime(); */
    SystemExitLoop(RETURN_GAME_UNPAUSE);
    break;
  }
}

void initPause() {
  SystemHidePointer();
  SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);

  /* disable game sound effects */
#ifdef SOUND
  Audio_DisableEngine();
#endif

  /* 
   * TODO: Provide an option to disable game music here. 
   * Game should be totally silent in pause mode. (Nice when 
   * the boss is walking by, phone call, etc...)
   */
  
  updateSettingsCache();
}

void exitPause() {
/* sound effects are re-enabled in initGame() */
//#ifdef SOUND  
// Audio_EnableEngine(); 
//#endif
}

void initPauseGL() {
  initGLGame();
}

callbacks pauseCallbacks = {
  displayPause, idlePause, keyboardPause,
  initPause, exitPause, initPauseGL, gameMouse, gameMouseMotion, "pause"
};

void keyboardPrompt(int key, int x, int y) {
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

void initPrompt() { }
void exitPrompt() { }

callbacks promptCallbacks = {
  displayPause, idlePause, keyboardPrompt,
  initPrompt, exitPrompt, NULL, NULL, NULL, "prompt"
};




