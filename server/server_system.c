/* system specific functions (basically, an SDL wrapper) */

#include "server_system.h"

void SystemInit(int *argc, char *argv[]) {
  if(SDL_Init(0) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL video: %s\n", SDL_GetError());
    exit(2);
  }

#ifdef NETWORK
  SystemNetInit();
#endif
}

int SystemGetElapsedTime() {
  /* fprintf(stderr, "%d\n", SDL_GetTicks()); */
  return SDL_GetTicks();
}
