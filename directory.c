#include <stdio.h>

/* there is a conflict with the point type and mac headers
    a better fix is to put code in separate files
*/
#include <stdlib.h>
#include <string.h>

#include "gltron.h"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

list* readDirectoryContents(const char *dirname, char *prefix) {
  DIR *dir;
  struct dirent *entry;
  list *l, *p;

  l = (list*) malloc(sizeof(list));
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
		p->next = (list*) malloc(sizeof(list));
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
