#ifndef NEBU_FILE_SYSTEM_H
#define NEBU_FILE_SYSTEM_H

#include "base/nebu_util.h"
#include "filesystem/nebu_file_io.h"

#include <stdio.h>
#include <stdlib.h>

extern void initFilesystem(int argc, const char *argv[]);

extern nebu_List* readDirectoryContents(const char *dirname, const char *prefix);

/* findpath.c, GPL'd code */
extern void goto_installpath(const char *executable);

extern void dirSetup(const char *executable);
extern int fileExists(const char *path);
extern void makeDirectory(const char* name);

#endif
