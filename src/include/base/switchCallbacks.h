#ifndef SWITCHCALLBACKS_H
#define SWITCHCALLBACKS_H

#include "Nebu_base.h"

extern Callbacks gameCallbacks;
extern Callbacks guiCallbacks;
extern Callbacks pauseCallbacks;
extern Callbacks configureCallbacks;
extern Callbacks promptCallbacks;
extern Callbacks creditsCallbacks;
extern Callbacks timedemoCallbacks;

extern void restoreCallbacks(void);
extern void switchCallbacks(Callbacks* callbacks);
extern void updateCallbacks(void);

extern void setCallback(const char *name);

#endif
