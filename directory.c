#include <stdio.h>
#include "gltron.h"

#ifdef macintosh
static char* subdir = ":Data";
#else 
static char* subdir = "data";
#endif

/* the following code doesn't compile under non-POSIX systems (they
   don't have dirent.h), so we do lots of special casing */

#ifdef WIN32

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

char *getMusicPath(char *name) {
  return getFullPath(name);
}

#elif defined macintosh

/* macintosh code by Darrell Walisser */

#include <IterateDirectory.h>
#include <FullPath.h>
#include <Types.h>
#include <Files.h>
#include <Processes.h>

/* -dw- From Technical Q&A FL14 */
/* http://developer.apple.com/qa/fl/fl14.html */

 /* GetApplicationDirectory returns the volume reference number
  and directory ID for the current application's directory. */
     
 OSStatus GetApplicationDirectory(short *vRefNum, long *dirID) {
     ProcessSerialNumber PSN;
     ProcessInfoRec pinfo;
     FSSpec pspec;
     OSStatus err;
         /* valid parameters */
     if (vRefNum == NULL || dirID == NULL) return paramErr;
         /* set up process serial number */
     PSN.highLongOfPSN = 0;
     PSN.lowLongOfPSN = kCurrentProcess;
         /* set up info block */
     pinfo.processInfoLength = sizeof(pinfo);
     pinfo.processName = NULL;
     pinfo.processAppSpec = &pspec;
         /* grab the vrefnum and directory */
     err = GetProcessInformation(&PSN, &pinfo);
     if (err == noErr) {
         *vRefNum = pspec.vRefNum;
         *dirID = pspec.parID;
     }
     return err;
 }

pascal void iterateProc (const CInfoPBRec * const cpb_ptr,
								  Boolean *quit_flag,
								  void *user_data) {
	char *filename;							  
   int   len;
   
   list *l = (list*) user_data;
	
   if (cpb_ptr->hFileInfo.ioFlAttrib == 2) {	
   	
   	while (l->next != NULL)
   	   l++;
   	
   	len = cpb_ptr->hFileInfo.ioNamePtr[0];
   	cpb_ptr->hFileInfo.ioNamePtr[len+1] = '\0';
   	filename = cpb_ptr->hFileInfo.ioNamePtr + 1;
   	
   	l->data = malloc (sizeof(char) * len + 1);  							  
      strcpy (l->data, filename);
      
      l->next = (list*) malloc (sizeof (list));
      l->next->next = NULL;
      l->next->data = NULL;
   }
}

list* readDirectoryContents(char *dirname, char *prefix) {

  list *l;
  OSErr err;
  char *path;
  
  Str255 relPath;  
  short vRefNum;
  long  dirID;
  FSSpec spec;
  
  int len = strlen(dirname) + 1;
  relPath[0] = len;
  memcpy (relPath + 1, dirname, len);
  relPath[ len ] = ':';

  err = GetApplicationDirectory (&vRefNum, &dirID);
  if (err != noErr) {
    fprintf (stderr, "GetApplicationDirectory failed\n");
    exit (-1);
  }
  
  err = FSMakeFSSpec  (vRefNum, dirID, relPath, &spec);
     
  if (err != noErr) {
    fprintf (stderr, "FSMakeFSSpec failed\n");
    exit (-1);
  }
  
  l = (list*) malloc(sizeof(list));
  l->data = NULL;
  l->next = NULL;
 
  err = FSpIterateDirectory (&spec, 1, iterateProc, l);
    
  if (err != noErr)  {
    fprintf (stderr, "FSpIterateDirectory failed\n");
    exit (-1);
  }
 
  return l;
}

char *getMusicPath(char *name) {
  char *path;
  path = malloc(strlen(subdir) + 1 + strlen(name) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, name);
  return path;
}

/* end of macintosh code */

#else

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
  char *path;
  path = malloc(strlen(subdir) + 1 + strlen(name) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, name);
  return path;
}

#endif

