// #include "gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef macintosh
#   include <unistd.h>
#endif

#ifdef macintosh
#    define SEPERATOR ':'
#endif

#ifdef WIN32
#    define SEPERATOR '\\'
#endif

#ifndef SEPERATOR
#    define SEPERATOR '/'
#endif

static int n_dirs = 2;

#ifdef macintosh 
static char *dirs[] = { ":Data", ":Art" };
#else
static char *dirs[] = { "data", "art" };
#endif

/* fopen() in MacOS on a directory returns NULL, here is a better solution */
/* that tells us if a file OR directory exists */
#ifdef macintosh

#include <Files.h>

extern OSStatus GetApplicationDirectory(short *vRefNum, long *dirID); /* in directory.c */

static int itemExists ( const char* path ) {
    
  OSStatus  err;
  Str255    relPath; 
  short     vRefNum;
  long      dirID;
  FSSpec    spec;

  if (*path != ':')
   sprintf (relPath, "%c:%s", (strlen(path) + 1), path);
  else 
   sprintf (relPath, "%c%s", (strlen(path)) , path);

  err = GetApplicationDirectory (&vRefNum, &dirID);
  if (err != noErr) {
    fprintf (stderr, "GetApplicationDirectory failed\n");
    exit (-1);
  }
  
  err = FSMakeFSSpec  (vRefNum, dirID, relPath, &spec);
  
  return (err == noErr);
}

#else /* UNIX, Windoze, MacOS X */

#include <unistd.h>

static int itemExists (const char* path) {

    return (access (path, F_OK) == 0);
}

#endif


char* getFullPath(char *filename) {
  char *path;

  int i;
  for(i = 0; i < n_dirs; i++) {
    path = malloc(strlen(dirs[i]) + 1 + strlen(filename) + 1);
    sprintf(path, "%s%c%s", dirs[i], SEPERATOR, filename);
    // printf("checking '%s'...", path);
    if (itemExists (path) ) {
      // printf("ok\n");
      return path;
    }
    free(path);
    // printf("unsuccessful\n");
  }
  return NULL;
}
