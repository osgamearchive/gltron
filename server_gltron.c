/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "server_gltron.h"

int main( int argc, char *argv[] ) {
  initMainGameSettings();
  /**
  initGameStructures();
  resetScores();
  initData();
  */
  //initNetwork();
  initNetwork();

  //starting the server
  start_server();
  
  while(game->pauseflag == 0) {
    idleGame();
    SDL_Delay(10);
  }
  stop_server();
  return 0;
}
