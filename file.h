#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

extern char* getFullPath(char *filename);
extern int   fileExists (const char* path);

#ifdef __cplusplus
}
#endif

#endif /* FILE_H */
