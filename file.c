#include "file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef macintosh
#   include <limits.h>
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

#if 0
static int n_dirs = 2;

#ifdef macintosh 
static char *dirs[] = { ":Data", ":Art" };
#else
static char *dirs[] = { "data", "art" };
#endif

#endif /* 0 */

/* fopen() in MacOS on a directory returns NULL, here is a better solution */
/* that tells us if a file OR directory exists */
#ifdef macintosh

#include <Files.h>

extern OSStatus GetApplicationDirectory(short *vRefNum, long *dirID); /* in directory.c */

int itemExists (const char* path) {
    
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

int itemExists (const char* path) {
    return (access (path, F_OK) == 0);
}

#endif

#if 0
char* getFullPath(const char *filename) {
  char *path;

  int i;
  for(i = 0; i < n_dirs; i++) {
    path = malloc(strlen(dirs[i]) + 1 + strlen(filename) + 1);
    sprintf(path, "%s%c%s", dirs[i], SEPERATOR, filename);
    /* printf("checking '%s'...", path); */
    if (itemExists (path) ) {
      /* printf("ok\n"); */
      return path;
    }
    free(path);
    /* printf("unsuccessful\n"); */
  }
  return NULL;
}
#endif

#ifndef PATH_MAX
#warning PATH_MAX "is not defined in limits.h!"
#define PATH_MAX 255
#endif

static char preferences_dir[PATH_MAX];
static char snapshots_dir[PATH_MAX];
static char data_dir[PATH_MAX];
static char art_dir[PATH_MAX];
static char music_dir[PATH_MAX];
static char scripts_dir[PATH_MAX];

char* getHome() {
  return getenv("HOME");
}

void initDirectories() {
  if(PREF_DIR[0] != '~')
    sprintf(preferences_dir, PREF_DIR);
  else
    sprintf(preferences_dir, "%s%s", getHome(), PREF_DIR + 1);

  if(SNAP_DIR[0] != '~')
    sprintf(snapshots_dir, SNAP_DIR);
  else
    sprintf(snapshots_dir, "%s%s", getHome(), SNAP_DIR + 1);

#ifdef LOCAL_DATA
  sprintf(data_dir, "data");
  sprintf(art_dir, "art");
  sprintf(scripts_dir, "scripts");
#else
  sprintf(data_dir, "%s%c%s", DATA_DIR, SEPERATOR, "data");
  sprintf(art_dir, "%s%c%s", DATA_DIR, SEPERATOR, "art");
  sprintf(scripts_dir, "%s%c%s", DATA_DIR, SEPERATOR, "scripts");
#endif
  sprintf(music_dir, "%s%c" "music", data_dir, SEPERATOR);


  printf("directories:\n"
	 "\tprefs: %s\n"
	 "\tsnaps: %s\n"
	 "\tdata:  %s\n"
	 "\tart:   %s\n"
	 "\tscripts:   %s\n"
	 "\tmusic: %s\n",
	 preferences_dir, snapshots_dir, 
	 data_dir, art_dir, scripts_dir, 
	 music_dir);
}

char* getPath( int eLocation, const char *filename) {
  char *path = getPossiblePath( eLocation, filename );
  if( fileExists(path) )
    return path;


  fprintf(stderr, "*** failed to locate file '%s' at '%s' (type %d)\n",
	  filename, path, eLocation);
  assert(0);

  free(path);
  return NULL;
}

char* getPossiblePath( int eLocation, const char *filename ) {
  char *path = malloc( PATH_MAX );
  sprintf(path, "%s%c%s", getDirectory( eLocation ), SEPERATOR, filename);
  return path;
}

const char* getDirectory( int eLocation ) {
  switch( eLocation ) {
  case PATH_PREFERENCES: return preferences_dir; break;
  case PATH_SNAPSHOTS: return snapshots_dir; break;
  case PATH_DATA: return data_dir; break;
  case PATH_SCRIPTS: return scripts_dir; break;
  case PATH_MUSIC: return music_dir; break;
  default:
    fprintf(stderr, "invalid path type\n");
    assert(0);
  }
}
char *getArtPath(const char *artpack, const char *filename ) {
  char *path = malloc( PATH_MAX );
  sprintf(path, "%s%c%s%c%s", 
	  art_dir, SEPERATOR, artpack, SEPERATOR, filename);
  if( fileExists(path) )
    return path;

  sprintf(path, "%s%c%s%c%s", 
	  art_dir, SEPERATOR, "default", SEPERATOR, filename);
  if( fileExists(path) )
    return path;

  fprintf(stderr, "*** failed to locate art file '%s', giving up\n", filename);
  assert(0);

  free(path);
  return NULL;
}
    
int fileExists(const char *path) {
  FILE* f;
  if((f = fopen(path, "r"))) {
    fclose(f);
    return 1;
  }
  return 0;
}


















