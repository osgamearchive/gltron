#ifndef GLTRON_H
#define GLTRON_H

/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#define RC_NAME ".gltronrc"

/* Win32 port maintained by Andreas Umbach <marvin@dataway.ch> */
#ifdef WIN32
#include <windows.h>

#undef RC_NAME
#define RC_NAME "gltron.ini"

#endif /* WIN32 */

/* MacOS port maintained by Darrell Walisser <walisser@mac.com> */

#ifdef macintosh  /* os9/classic */
#include <string.h>
#undef RC_NAME
#define RC_NAME "gltronPrefs.txt"
#endif 

#ifdef __APPLE__ /* osx */
#undef RC_NAME
#define RC_NAME "gltronPrefs.txt"
#endif

/* the engine */
#include "Nebu.h"

/* subsystems */
#include "game/game.h"
#include "configuration/configuration.h"
#include "video/video.h"
#include "audio/audio.h"
#include "input/input.h"

#endif /* GLTRON_H */
