#ifdef WIN32

#include "base/nebu_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* FIXME: This is really broken. */
List* readDirectoryContents(const char *dirname, const char *prefix) {
	WIN32_FIND_DATA search;
	HANDLE hDir;
	List *currentList;
	char *file;

	currentList = malloc(sizeof(List));
	currentList->next = malloc(sizeof(List));
	currentList->next->next = NULL;

	hDir = FindFirstFile(dirname, &search);
	if(hDir == 0)
	{
		return NULL;
	}
	file = malloc(strlen(search.cFileName) + 1);
	strcpy(file,search.cFileName);
	
	currentList->next->data = file;
	currentList->data = file;


	return currentList;

}

void makeDirectory(const char *name) {
	;
}

#endif
