#ifndef GRAPHICS_UTILITY_H
#define GRAPHICS_UTILITY_H

#include "video.h"
#include "video/nebu_font.h"

extern void rasonly(Visual *d);
extern void doPerspective(float fov, float ratio, float znear, float zfar);
extern void doLookAt(float *cam, float *target, float *up);
extern void drawText(nebu_Font* ftx, float x, float y, float size, const char *text);

#endif
