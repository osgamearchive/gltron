#include "base/switchCallbacks.h"
#include <string.h>
#include <stdlib.h>

Callbacks *last_callback = NULL;
Callbacks *current_callback = NULL;

void exitCallback(Callbacks *cb) {
  if(cb != NULL)
    if(cb->exit != NULL)
    (cb->exit)(); /* give them the chance to quit */
}

void initCallback(Callbacks *cb) {
  if(cb->init != NULL)
    (cb->init)();
  if(cb->initGL != NULL)
    (cb->initGL)();
}


void switchCallbacks(Callbacks *new) {
	// if(current_callback)
	// fprintf(stderr, "callbacks: exiting %s\n", current_callback->name);
  exitCallback(current_callback);
  SystemRegisterCallbacks(new);
	// fprintf(stderr, "callbacks: initializing %s\n", new->name);
  initCallback(new);

  last_callback = current_callback;
  current_callback = new;
}
  
void updateCallbacks(void) {
  /* called when the window is recreated */
  exitCallback(current_callback);
  SystemRegisterCallbacks(current_callback);
  initCallback(current_callback);
}

void restoreCallbacks(void) {
  if(last_callback == NULL) {
    fprintf(stderr, "fatal: no last callback present, exiting\n");
    exit(1); // OK: programmer error, critical
  }

  exitCallback(last_callback);
  current_callback = last_callback;
  SystemRegisterCallbacks(current_callback);
  initCallback(current_callback);
}

#define N_CALLBACKS 7
Callbacks *callbackList[N_CALLBACKS] = {
	&gameCallbacks, &guiCallbacks, &pauseCallbacks, &configureCallbacks,
	&promptCallbacks, &creditsCallbacks, &timedemoCallbacks
};

void setCallback(const char *name) {
	int i;

	for(i = 0; i < N_CALLBACKS; i++) {
		if(strcmp( callbackList[i]->name, name ) == 0)
			break;
	}
	if(i == N_CALLBACKS) {
		fprintf(stderr, "fatal: no callback named '%s' found\n", name);
		exit(1); // OK: programmer error, critical
	}
	switchCallbacks(callbackList[i]);
}


