#include "gltron.h"

callbacks *last_callback = NULL;
callbacks *current_callback = NULL;

void switchCallbacks(callbacks *new) {
  last_callback = current_callback;
  current_callback = new;

  if(last_callback != NULL)
    (last_callback->exit)(); /* give them the chance to quit */

  SystemRegisterCallbacks(new);

  /* lasttime = SystemGetElapsedTime(); */

 /* printf("callbacks registred\n"); */
  (new->init)();
  (new->initGL)();
  /* printf("callback init's completed\n"); */
}
  
void updateCallbacks() {
  /* called when the window is recreated */
  SystemRegisterCallbacks(current_callback);

  (current_callback->initGL)();
}

void restoreCallbacks() {
  if(last_callback == NULL) {
    fprintf(stderr, "no last callback present, exiting\n");
    exit(1);
  }
  current_callback = last_callback;

  SystemRegisterCallbacks(current_callback);
  
  /* lasttime = SystemGetElapsedTime(); */

  fprintf(stderr, "restoring callbacks\n");
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


