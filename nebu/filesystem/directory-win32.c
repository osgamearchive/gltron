#ifdef WIN32

#include "base/nebu_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int isHiddenFile(WIN32_FIND_DATA *search)
{
	if(search->cFileName[0] == '.')
	{
		return 1;
	}else 
	{
		return search->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;
	}
}

/* FIXME: This is really broken. */
nebu_List* readDirectoryContents(const char *dirname, const char *prefix) {
	WIN32_FIND_DATA search;
	HANDLE hSearch;
	nebu_List *l, *p;
	char *filename, *searchStr;
	int   len, prefixLen = 0;

	p = malloc(sizeof(nebu_List));
	p->next= NULL;
	l = p;

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
	}else if(!isHiddenFile(&search))
	{
		len = strlen(search.cFileName) + 1;
		filename = malloc(len);
		memcpy(filename, search.cFileName, len);
		p->data = filename;
		p->next = malloc(sizeof(nebu_List));
		p = p->next;
		p->next = NULL;

	}

	while( FindNextFile(hSearch, &search) )
	{
		if(!isHiddenFile(&search))
		{
			len = strlen(search.cFileName) + 1;
			filename = malloc(len);
			memcpy(filename, search.cFileName, len);
			p->data = filename;
			p->next = malloc(sizeof(nebu_List));
			p = p->next;
			p->next = NULL;
		}
	}

	if(GetLastError() != ERROR_NO_MORE_FILES)
	{
		fprintf(stderr, "[directory] Error: something bad happened\n");
	}
	

	FindClose(hSearch);
	free(searchStr);

	return l;

}

void makeDirectory(const char *name) {
	;
}

#endif
