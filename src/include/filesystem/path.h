#ifndef PATH_H
#define PATH_H

extern const char* getDirectory(int eLocation);
extern char* getPath(int eLocation, const char *filename);
extern char* getPossiblePath(int eLocation, const char *filename);
extern char* getArtPath(const char *artpack, const char *filename);
extern void initDirectories(void);

/* if you update this listing, also make sure you update scripts/path.lua */

enum { 
  PATH_PREFERENCES = 0,
  PATH_SNAPSHOTS, /* demos, screenshots */
  PATH_DATA,
  PATH_SCRIPTS,
  PATH_MUSIC,
  PATH_ART,
	PATH_LEVEL
};

#endif
