#ifndef FILE_SYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>

#include "basic_types.h" // defines List datatype

/* basic io wrapper */
#include "file_io.h"

/* file handling -> file.c */
#include "file.h"

void initFilesystem(int argc, const char *argv[]);

extern List* readDirectoryContents(const char *dirname, const char *prefix);

/* findpath.c, GPL'd code */
extern void goto_installpath(const char *executable);

void dirSetup(const char *executable);

#endif
