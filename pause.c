#include "gltron.h"

/* very brief - just the pause mode */

// static float Gamma = 2.0;

void idlePause() {
#ifdef __NETWORK__
  int sockstat;
#endif


#ifdef SOUND
  soundIdle();
#endif

#ifdef __NETWORK__
  if( game2->mode == GAME_NETWORK_PLAY && serverstate == gameState )
    {
      nbSynch = 1;
      synchCount = 0;
      initTurnList();
      switchCallbacks(&gameCallbacks);
    } else {
      if( game2->mode == GAME_NETWORK_PLAY && isConnected )
	{
	  sockstat = Net_checksocks();
	  if( sockstat != socksnotready ) 
	    {
	      if( sockstat & tcpsockready )
		handleServer();
#ifdef USEUDP
	      if( sockstat & udpsockready )
		printf("getting udp\n");
#endif
	    }
	}
    }
#endif

  /* 
  if(SystemGetElapsedTime() - lasttime < 10) return;
  timediff();
  */
  doCameraMovement();
  if(game->settings->screenSaver && stoptime != 0 &&
     SystemGetElapsedTime() - stoptime > 2000) {
    initData();
    stoptime = 0;
    switchCallbacks(&gameCallbacks);
  }
  SystemPostRedisplay();
}

void displayPause() {
  drawGame();
  drawPause(game->screen);

  SystemSwapBuffers();
}

void keyboardPause(int key, int unicode, int x, int y) {
  int      i;
#ifdef __NETWORK__
  Packet   packet;
#endif

  switch(key) {
  case 27:
#ifdef __NETWORK__
    if( game2->mode == GAME_NETWORK_PLAY && isConnected )
      {
	isConnected = 0;
	isLogged = 0;
	serverstate=preGameState; //We hope that next time server will be to preGameState
	Net_disconnect();
      }
#endif
    switchCallbacks(&guiCallbacks);
    break;
  case ' ':
#ifdef __NETWORK__
    //restart game
    if( game2->mode == GAME_NETWORK_PLAY && (serverstate == preGameState) && (isConnected && slots[me].isMaster == 1)  )
      {
	makeping(0);
	printf("\nAsk to start the game\n");
	packet.which=me;
	packet.type=ACTION;
	packet.infos.action.type=STARTGAME;
	Net_sendpacket(&packet,  Net_getmainsock());
      } else {
	if( game2->mode == GAME_NETWORK_PLAY )
	    printf("\nOnly game master can start the game...\n");
	else {
#endif	
	    if(game->pauseflag & PAUSE_GAME_FINISHED)
	      initData();
	    /* lasttime = SystemGetElapsedTime(); */
	    switchCallbacks(&gameCallbacks);
#ifdef __NETWORK__
	  }
      }
#endif   
    break;
    /* case 'q': SystemExit(); break; */
  case SYSTEM_KEY_F1: defaultDisplay(0); break;
  case SYSTEM_KEY_F2: defaultDisplay(1); break;
  case SYSTEM_KEY_F3: defaultDisplay(2); break;
  case SYSTEM_KEY_F4: defaultDisplay(3); break;
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
    game->settings->camType = (game->settings->camType + 1) % CAM_COUNT;
    for(i = 0; i < game->players; i++)
      if(game->player[i].ai->active == 0)
	initCamera(game->player[i].camera, 
		   game->player[i].data, 
		   game->settings->camType);
    break;
  case SYSTEM_KEY_F12: doScreenShot(game->screen->vp_w, game->screen->vp_h); break;
    
  case SYSTEM_KEY_UP: consoleScrollBackward(1); break;
  case SYSTEM_KEY_DOWN: consoleScrollForward(1); break;

  case SYSTEM_KEY_F5: saveSettings(); break;
  case SYSTEM_KEY_TAB: switchCallbacks(&promptCallbacks); break;
  }
}

void initPause() {
  SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
  fprintf(stderr, "init pause mode\n");
  // Mix_SetPostMix(playEngine, NULL);
}

void exitPause() {
  // Mix_SetPostMix(NULL, NULL);
}

void initPauseGL() {
  initGLGame();
}

callbacks pauseCallbacks = {
  displayPause, idlePause, keyboardPause,
  initPause, exitPause, initPauseGL, gameMouse, gameMouseMotion
};

void keyboardPrompt(int key, int unicode, int x, int y) {
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




