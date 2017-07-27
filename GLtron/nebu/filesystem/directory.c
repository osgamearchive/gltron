#include "base/nebu_util.h"
#include "base/nebu_debug_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#ifdef WIN32
#include "filesystem/nebu_dirent.h"
#else
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <direct.h>
#endif

nebu_List* readDirectoryContents(const char *dirname, const char *prefix) {
  DIR *dir;
  struct dirent *entry;
  nebu_List *l, *p;

  l = (nebu_List*) malloc(sizeof(nebu_List));
  p = l;
  p->next = NULL;

  dir = opendir(dirname);
  if(dir == NULL) {
    nebu_Log("warning: cannot open directory '%s'\n", dirname);
    return l;
  }
  while((entry = readdir(dir)) != NULL) {
    char *name;
    if(prefix == NULL || strstr(entry->d_name, prefix) == entry->d_name) {
      if(entry->d_name[0] != '.') {
		name = malloc(strlen(entry->d_name) + 1);
		memcpy(name, entry->d_name, strlen(entry->d_name) + 1);
		p->data = name;
		p->next = (nebu_List*) malloc(sizeof(nebu_List));
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
#ifndef WIN32
  if(access(name, R_OK)) {
    result = mkdir(name, 0x1ff);
#else
  if(_access(name, 4)) {
    result = _mkdir(name);
#endif
    if(result)
      nebu_Log("cannot create dir '%s': %s\n", name, strerror(errno));
  }
}
