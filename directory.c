#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include "gltron.h"

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

