/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "server_system.h"
#include "server_data.h"
#include "protocol.h"
#include "tracker_protocol.h"


#ifndef GLTRON_TRACKER_H
#define GLTRON_TRACKER_H

/** Settings */
typedef struct {
  char     passwd[9];      //tracker's passwd
  int      port;           //tracker port
  char     tracker[33];    //tracker's name
  int      timeout;        //that is timeout for a server.
} Tracker_settings;

extern Tracker_settings settings;
extern char     configpath[32];

/** Settings functions */
extern void get_args( int argc, char *argv[] );
extern void initDefaultSettings();
extern void ReadConfigFile();

/** Network functions */
extern void initNetwork();
extern void start_tracker();
extern void stop_tracker();
extern void handle_tracker();
extern void check_serveractivity();

/** slots */
extern Trackerslots  slots[MAX_SLOTS];

#endif
