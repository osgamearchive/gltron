#include <stdio.h>


#ifdef macintosh

/* there is a conflict with the point type and mac headers
    a better fix is to put code in separate files
*/
#include <stdlib.h>
#include <string.h>

typedef struct list list;
struct list {
  void *data;
  list* next;
};

#define SEPERATOR ':'

#else
#include "gltron.h"
#endif


#ifdef macintosh
static char* subdir = ":Data";
#else 
static char* subdir = "data";
#endif

/* the following code doesn't compile under non-POSIX systems (they
   don't have dirent.h), so we do lots of special casing */

#ifdef macintosh

/* macintosh code by Darrell Walisser */

#include <IterateDirectory.h>
#include <FullPath.h>
#include <Types.h>
#include <Files.h>
#include <Processes.h>

static char *filter_prefix = NULL;

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
   StringPtr filename;							  
   int   len;
   
   list *l = (list*) user_data;
	/* filter invisible files & folders */
   if ( (cpb_ptr->hFileInfo.ioFlFndrInfo.fdFlags & kIsInvisible) == 0) {	
   	
   	len = cpb_ptr->hFileInfo.ioNamePtr[0];
   	cpb_ptr->hFileInfo.ioNamePtr[len+1] = '\0';
   	filename = cpb_ptr->hFileInfo.ioNamePtr + 1;
   	
   	/* filter file names if necessary */
   	if ( filter_prefix && (strstr ((const char*)filename, filter_prefix) != (const char*)filename) )
   		return;
   	
   	while (l->next != NULL)
   	   l=l->next;
   	
   	l->data = (char*) malloc (sizeof(char) * len + 1);  							  
      strcpy ((char*)l->data, (const char*)filename);
      
      l->next = (list*) malloc (sizeof (list));
      l->next->next = NULL;
      l->next->data = NULL;
   }
}

list* readDirectoryContents(const char *dirname, char *prefix) {

  list *l;
  OSErr err;
  char *path;
  
  Str255 relPath;  
  short vRefNum;
  long  dirID;
  FSSpec spec;

  if (*dirname != ':')
    sprintf ((char*)relPath, "%c:%s", strlen(dirname) + 1, dirname);
  else
    sprintf ((char*)relPath, "%c%s", strlen(dirname) , dirname);

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
 
  filter_prefix = prefix;
  
  err = FSpIterateDirectory (&spec, 1, iterateProc, l);
    
  if (err != noErr)  {
    fprintf (stderr, "FSpIterateDirectory failed\n");
    exit (-1);
  }
 
  return l;
}

char *getMusicPath(char *name) {
  char *path;
  path = (char*)malloc(strlen(subdir) + 1 + strlen(name) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, name);
  return path;
}

/* end of macintosh code */

#else

#include <sys/types.h>
#include <dirent.h>

list* readDirectoryContents(const char *dirname, char *prefix) {
  DIR *dir;
  struct dirent *entry;
  list *l, *p;

  l = (list*) malloc(sizeof(list));
  p = l;
  p->next = NULL;

  dir = opendir(dirname);
  if(dir == NULL) {
    fprintf(stderr, "warning: cannot find music directory\n");
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

char *getMusicPath(char *name) {
  char *path;
  path = malloc(strlen(subdir) + 1 + strlen(name) + 1);
  sprintf(path, "%s%c%s", subdir, SEPERATOR, name);
  return path;
}

#endif

