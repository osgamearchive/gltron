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

static int n_dirs = 2;

#ifdef macintosh 
static char *dirs[] = { ":Data", "art" };
#else
static char *dirs[] = { "data", "art" };
#endif

char* getFullPath(char *filename) {
  char *path;
  FILE *fp = NULL;

  int i;
  for(i = 0; i < n_dirs; i++) {
    path = malloc(strlen(dirs[i]) + 1 + strlen(filename) + 1);
    sprintf(path, "%s%c%s", dirs[i], SEPERATOR, filename);
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
  return NULL;
}
