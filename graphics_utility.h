#ifndef GRAPHICS_UTILITY_H
#define GRAPHICS_UTILITY_H

#include "video.h"

extern void checkGLError(const char *where);
extern void rasonly(Visual *d);
extern void doPerspective(float fov, float ratio, float znear, float zfar);
extern void doLookAt(float *cam, float *target, float *up);
extern void drawText(FontTex* ftx, int x, int y, int size, const char *text);

#endif
