// #include "gltron.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef macintosh
#    define SEPERATOR ':'
#endif

#ifdef WIN32
#    define SEPERATOR '\\'
#endif

#ifndef SEPERATOR
#    define SEPERATOR '/'
#endif

/* #ifdef UNIX */
static char *share1 = "/usr/share/games/gltron";
static char *share2 = "/usr/local/share/games/gltron";
/* #endif */

#ifdef macintosh 
static char *subdir = ":Data";
#else
static char* subdir = "data";
#endif

#if defined WIN32 || defined macintosh 
/* assume current directory + data/ */
char* getFullPath(char *filename) {
  char *path;
  fprintf(stderr, "using win32/mac hack looking for %s\n", filename);
  path = malloc(strlen(subdir) + 1 + strlen(filename) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, filename);

  return path;
}
#else

char* getFullPath(char *filename) {
  char *path;
  FILE *fp = NULL;
  char *base;

  /* check a few directories for the files and */
  /* return the full path. */
  
  /* check: data sub-directory, GLTRON_HOME, and */
  /* /usr/share/games/gltron and /usr/local/share/games/gltron */

  path = malloc(strlen(subdir) + 1 + strlen(filename) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, filename);

  printf("checking '%s'...", path);
  fp = fopen(path, "r");
  if(fp != NULL) {
    fclose(fp);
	printf("ok\n");
    return path;
  }
  free(path);
  printf("unsuccessful\n");

  base = getenv("GLTRON_HOME");
  if(base != NULL) {
    path = malloc(strlen(base) + 1 + strlen(filename) + 1);
    sprintf(path, "%s%c%s", base, SEPERATOR, filename);

    printf("checking '%s'...", path);
    fp = fopen(path, "r");
    if(fp != NULL) {
      fclose(fp);
	  printf("ok\n");
      return path;
    }
    free(path);
    printf("unsuccessful\n");
  }

  path = malloc(strlen(share1) + 1 + strlen(filename) + 1);
  sprintf(path, "%s%c%s", share1, SEPERATOR, filename);

  printf("checking '%s'", path);
  fp = fopen(path, "r");
  if(fp != NULL) {
	printf("ok\n");
    fclose(fp);
    return path;
  }
  free(path);
  printf("unsuccessful\n");

  path = malloc(strlen(share2) + 1 + strlen(filename) + 1);
  sprintf(path, "%s%c%s", share2, SEPERATOR, filename);
  
  printf("checking '%s'", path);
  fp = fopen(path, "r");
  if(fp != NULL) {
    fclose(fp);
	printf("ok\n");
    return path;
  }  
  free(path);
  printf("unsuccessful\n");

  return NULL;
}

#endif  
  

  
