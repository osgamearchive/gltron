#ifndef GLTRON_H
#define GLTRON_H

/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <limits.h>

#define RC_NAME ".gltronrc"

#ifndef M_PI
#define M_PI 3.141592654
#endif

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

/* subsystems */
#include "filesystem.h"
#include "scripting.h"
#include "game.h"
#include "configuration.h"
#include "video.h"
#include "audio.h"
#include "input.h"

#include "init.h"
#include "system.h"

#ifdef __cplusplus
}
#endif /* extern C */

#endif /* GLTRON_H */
