#include "base/nebu_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

List* readDirectoryContents(const char *dirname, const char *prefix) {
  DIR *dir;
  struct dirent *entry;
  List *l, *p;

  l = (List*) malloc(sizeof(List));
  p = l;
  p->next = NULL;

  dir = opendir(dirname);
  if(dir == NULL) {
    fprintf(stderr, "warning: cannot open directory '%s'\n", dirname);
    return l;
  }
  while((entry = readdir(dir)) != NULL) {
    char *name;
    if(prefix == NULL || strstr(entry->d_name, prefix) == entry->d_name) {
      if(entry->d_name[0] != '.') {
		name = malloc(strlen(entry->d_name) + 1);
		memcpy(name, entry->d_name, strlen(entry->d_name) + 1);
		p->data = name;
		p->next = (List*) malloc(sizeof(List));
		p = p->next;
		p->next = NULL;
      }
    }
  }
  closedir(dir);
  return l;

}

void makeDirectory(const char *name) {
  int result;
  if(access(name, R_OK)) {
#ifndef WIN32
    result = mkdir(name, 0x1ff);
#else
    result = mkdir(name);
#endif
    if(result)
      printf("cannot create dir '%s': %s\n", name, strerror(errno));
  }
}
