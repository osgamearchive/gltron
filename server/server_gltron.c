/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "server_gltron.h"

int main( int argc, char *argv[] ) {
  int lastsent;

  get_args(argc, argv);
  
  
  initServerDefaultSettings();

  //read settings file
  ReadConfigFile();

  initMainGameSettings();
  initGameStructures();
  resetScores();
  initData();
  initNetwork();		

  //starting the server
  start_server();

  if( strcmp(settings.tracker, "NO") )
    {
      //connect to the server.
      tracker_connect();
      lastsent = SystemGetElapsedTime();
    }
  

  while(1) {
    idleGame();
    SDL_Delay(1);
    
    // On OS 9, we have to give up the processor every now and then
   #ifdef macintosh
    mac_yield_cpu ();
   #endif
   if( strcmp(settings.tracker, "NO") )
    {
      if( ( SystemGetElapsedTime() - lastsent ) > REFRESH*1000 )
	{
	  //send again infos.
	  tracker_sendinfos();
	  lastsent = SystemGetElapsedTime();
	}
    } 
  }
  stop_server();
  return 0;
}
