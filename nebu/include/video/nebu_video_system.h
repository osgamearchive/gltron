#ifndef NEBU_VIDEO_SYSTEM_H
#define NEBU_VIDEO_SYSTEM_H

#define SYSTEM_RGBA 1
#define SYSTEM_DOUBLE 2
#define SYSTEM_DEPTH 4
#define SYSTEM_FULLSCREEN 8
#define SYSTEM_32_BIT 16
#define SYSTEM_STENCIL 32

extern void SystemHidePointer();
extern void SystemUnhidePointer();
extern void SystemReshapeFunc(void(*reshape)(int, int));
extern int SystemWriteBMP(char *filename, int x, int y, unsigned char *pixels);
extern void SystemSetGamma(float r, float g, float b);

void nebu_Video_Init(void); // test ok

void nebu_Video_SetWindowMode(int x, int y, int w, int h); // test ok
void nebu_Video_SetDisplayMode(int flags); // test ok
int nebu_Video_Create(char *name); // test ok
void nebu_Video_Destroy(int id); // test ok

void nebu_Video_WarpPointer(int x, int y);
void nebu_Video_CheckErrors(const char *where);

#endif
