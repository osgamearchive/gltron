#ifndef SWITCHCALLBACKS_H
#define SWITCHCALLBACKS_H

#include "callbacks.h"

extern Callbacks gameCallbacks;
extern Callbacks guiCallbacks;
extern Callbacks pauseCallbacks;
extern Callbacks configureCallbacks;
extern Callbacks promptCallbacks;
extern Callbacks creditsCallbacks;
extern Callbacks timedemoCallbacks;

extern void restoreCallbacks();
extern void switchCallbacks(Callbacks* callbacks);
extern void updateCallbacks();

extern void setCallback(const char *name);

#endif
