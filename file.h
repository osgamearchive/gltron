#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum { 
    PATH_PREFERENCES = 0,
    PATH_SNAPSHOTS, /* demos, screenshots */
    PATH_DATA,
    PATH_SCRIPTS,
    PATH_MUSIC
  } Location;

#if 0	
extern char* getFullPath(char *filename);
#endif

extern int fileExists(char *path);
extern void initDirectories();

extern const char* getDirectory(int eLocation);
extern char* getPath(int eLocation, char *filename);
extern char* getPossiblePath(int eLocation, char *filename);
extern char* getArtPath(char *artpack, char *filename);

#ifdef __cplusplus
}
#endif

#endif /* FILE_H */
