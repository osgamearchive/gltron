/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "tracker_gltron.h"

int main( int argc, char *argv[] ) {
  //get args
  get_args(argc, argv);
  
  
  initDefaultSettings();

  //read settings file
  ReadConfigFile();

  //init system
  SystemInit(&argc,argv);
  initNetwork();

  //starting the tracker
  start_tracker();
  

  while(1) {
    //parse network
    handle_tracker();
    SDL_Delay(1);
  }

  //stop_server();

  //cleanup
  return 0;
}
