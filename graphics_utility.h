#ifndef GRAPHICS_UTILITY_H
#define GRAPHICS_UTILITY_H

#include "gltron.h"

extern void checkGLError(char *where);
extern void rasonly(gDisplay *d);
extern void doPerspective(float fov, float ratio, float znear, float zfar);
extern void doLookAt(float *cam, float *target, float *up);
extern void drawText(fonttex* ftx, int x, int y, int size, char *text);

#endif
