/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "server_gltron.h"

int main( int argc, char *argv[] ) {
  if(argc != 2) {
    //   fprintf(stderr, "usage: server <port-number>\n");
    //exit(1);
  } else {
    server_port = strtol(argv[1], (char**) NULL, 10);
  }


  initMainGameSettings();
  initGameStructures();
  resetScores();
  initData();
  initNetwork();

  //starting the server
  start_server();
  
  //while(game->pauseflag == 0) {
  while(1) {
    idleGame();
    SDL_Delay(10);
  }
  stop_server();
  return 0;
}
