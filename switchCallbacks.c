// #include "gltron.h"
#include "system.h"
#include "callbacks.h"

callbacks *last_callback = NULL;
callbacks *current_callback = NULL;

void exitCallback(callbacks *cb) {
  if(cb != NULL)
    if(cb->exit != NULL)
    (cb->exit)(); /* give them the chance to quit */
}

void initCallback(callbacks *cb) {
  if(cb->init != NULL)
    (cb->init)();
  if(cb->initGL != NULL)
    (cb->initGL)();
}


void switchCallbacks(callbacks *new) {
  exitCallback(last_callback);
  SystemRegisterCallbacks(new);
  initCallback(new);

  last_callback = current_callback;
  current_callback = new;
}
  
void updateCallbacks() {
  /* called when the window is recreated */
  exitCallback(current_callback);
  SystemRegisterCallbacks(current_callback);
  initCallback(current_callback);
}

void restoreCallbacks() {
  if(last_callback == NULL) {
    fprintf(stderr, "fatal: no last callback present, exiting\n");
    exit(1);
  }

  exitCallback(last_callback);
  current_callback = last_callback;
  SystemRegisterCallbacks(current_callback);
  initCallback(current_callback);
}

void chooseCallback(char *name) {
  /* maintain a table of names of callbacks */
  /* lets hardcode the names for all known modes in here */

  /* TODO(3): incorporate model stuff */
  /*
    if(strcmp(name, "chooseModel") ==  0) {
    fprintf(stderr, "change callbacks to chooseModel\n");
    switchCallbacks(&chooseModelCallbacks);
    }
  */
  if(strcmp(name, "gui") == 0) {
    /* fprintf(stderr, "change callbacks to gui\n"); */
    switchCallbacks(&guiCallbacks);
  }
}

void changeCallback(callbacks *new) {
  exitCallback(last_callback);
  SystemRegisterCallbacks(new);
  initCallback(new);

  last_callback = &guiCallbacks;
  current_callback = new;
}
