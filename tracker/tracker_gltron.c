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
    check_serveractivity();
    #ifdef macintosh
        mac_yield_cpu (10000);
    #else
        SDL_Delay(10);
    #endif
  }

  stop_tracker();

  //cleanup
  return 0;
}
