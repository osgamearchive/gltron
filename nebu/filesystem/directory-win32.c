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
	HANDLE hSearch;
	List *l, *f;
	char *filename, *searchStr;
	int   len, prefixLen = 0;

	l = malloc(sizeof(List));
	l->next= NULL;
	f = l;

	if(prefix != NULL)
	{
		prefixLen = strlen(prefix);
	}
	searchStr = malloc(strlen(dirname) + prefixLen + 3);
	memcpy(searchStr, dirname, strlen(dirname) + 1);
	strcat(searchStr, "\\");
	if(prefix != NULL)
	{
		strcat(searchStr, prefix);
	}
	strcat(searchStr, "*");


	hSearch = FindFirstFile(searchStr, &search);
	if(hSearch == INVALID_HANDLE_VALUE)
	{
		free(l);
		free(searchStr);
		return NULL;
	}

	len = strlen(search.cFileName) + 1;
	filename = malloc(len);
	memcpy(filename, search.cFileName, len);
	l->data = filename;

	while( FindNextFile(hSearch, &search) )
	{
		l->next = malloc(sizeof(List));
		l = l->next;
		len = strlen(search.cFileName) + 1;
		filename = malloc(len);
		memcpy(filename, search.cFileName, len);
		l->data = filename;
		l->next = NULL;
	}

	if(GetLastError() != ERROR_NO_MORE_FILES)
	{
		fprintf(stderr, "[directory] Error: something bad happened\n");
	}
	

	FindClose(hSearch);
	free(searchStr);

	return f;

}

void makeDirectory(const char *name) {
	;
}

#endif
