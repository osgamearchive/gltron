#ifndef NEBU_VIDEO_SYSTEM_H
#define NEBU_VIDEO_SYSTEM_H

#define SYSTEM_RGBA 1
#define SYSTEM_DOUBLE 2
#define SYSTEM_DEPTH 4
#define SYSTEM_FULLSCREEN 8
#define SYSTEM_32_BIT 16
#define SYSTEM_STENCIL 32

extern void SystemPostRedisplay();

extern void SystemHidePointer();
extern void SystemUnhidePointer();

extern void SystemSetGamma(float r, float g, float b);
extern void SystemInitWindow(int x, int y, int w, int h);
extern void SystemInitDisplayMode(int flags, unsigned char fullscreen);
extern int SystemCreateWindow(char *name);
extern void SystemDestroyWindow(int id);
extern void SystemReshapeFunc(void(*reshape)(int, int));

extern int SystemWriteBMP(char *filename, int x, int y, unsigned char *pixels);

void nebu_Video_WarpPointer(int x, int y);
void nebu_Video_Init(void);
void nebu_Video_CheckErrors(const char *where);

#endif
