#ifndef SWITCHCALLBACKS_H
#define SWITCHCALLBACKS_H

extern void restoreCallbacks();
extern void switchCallbacks(Callbacks* callbacks);
extern void updateCallbacks();

extern void setCallback(const char *name);

#endif
