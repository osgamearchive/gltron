#include <stdio.h>
#include "gltron.h"

static char* subdir = "data";

list *getSongList() {
  char *path;
  FILE *f;
  char buf[100];
  list *l, *p;

  path = getFullPath("music.list");
  if(path == NULL) {
    fprintf(stderr, "can't locate music.list, aborting\n");
    exit(2);
  }
  if((f = fopen(path, "r")) == NULL) {
    fprintf(stderr, "can't open music.list, aborting\n");
    exit(2);
  }
    
  free(path);

  l = (list*) malloc(sizeof(list));
  p = l;
  p->next = NULL;

  while(fgets(buf, sizeof(buf), f)) {
    char *name;
    name = malloc(strlen(buf));
    buf[ strlen(buf) - 1 ] = 0;
    memcpy(name, buf, strlen(buf) + 1);
    fprintf(stderr, "adding song '%s'\n", name);
    p->data = name;
    p->next = (list*) malloc(sizeof(list));
    p = p->next;
    p->next = NULL;
  }
  return l;
}

/* the following code doesn't compile under non-POSIX systems (they
   don't have dirent.h) */

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>

list* readDirectoryContents(char *dirname, char *prefix) {
  DIR *dir;
  struct dirent *entry;
  list *l, *p;

  l = (list*) malloc(sizeof(list));
  p = l;
  p->next = NULL;

  dir = opendir(dirname);
  while((entry = readdir(dir)) != NULL) {
    char *name;
    if(prefix == NULL || strstr(entry->d_name, prefix) == entry->d_name) {
      name = malloc(strlen(entry->d_name) + 1);
      memcpy(name, entry->d_name, strlen(entry->d_name) + 1);
      p->data = name;
      p->next = (list*) malloc(sizeof(list));
      p = p->next;
      p->next = NULL;
    }
  }
  closedir(dir);
  return l;

}

char *getMusicPath(char *name) {
  return getFullPath(name);
}

#else 
char *getMusicPath(char *name) {
  char *path;
  path = malloc(strlen(subdir) + 1 + strlen(name) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, name);
  return path;
}
#endif

