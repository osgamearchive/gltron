#ifndef SWITCHCALLBACKS_H
#define SWITCHCALLBACKS_H

#include "base/nebu_callbacks.h"

extern Callbacks gameCallbacks;
extern Callbacks guiCallbacks;
extern Callbacks pauseCallbacks;
extern Callbacks configureCallbacks;
extern Callbacks promptCallbacks;
extern Callbacks creditsCallbacks;
extern Callbacks timedemoCallbacks;

void restoreCallbacks(void);
void switchCallbacks(Callbacks* callbacks);
void updateCallbacks(void);

void setCallback(const char *name);

#endif
