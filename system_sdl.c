/* system specific functions (basically, an SDL wrapper) */

#include "system.h"

static int redisplay = 0;
static callbacks *current = 0;
static SDL_Surface *screen;
static int width, height;
static int flags;
static int fullscreen;

void SystemExit() {
  fprintf(stderr, "shutting down sound now\n");
#ifdef SOUND
  shutdownSound();
#endif
  fprintf(stderr, "shutting down sdl now\n");
  SDL_Quit();
  fprintf(stderr, "exiting application\n");
  exit(0);
}

void SystemInit(int *argc, char *argv[]) {
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SystemExit);
  SDL_EnableKeyRepeat(0, 0); /* turn keyrepeat off */
}

void SystemPostRedisplay() {
  redisplay = 1;
}

int SystemGetElapsedTime() {
  return SDL_GetTicks();
}

void SystemSwapBuffers() {
  SDL_GL_SwapBuffers();
}

void SystemWarpPointer(int x, int y) {
  SDL_WarpMouse(x, y);
}

void SystemMainLoop() {
  SDL_Event event;
  char *keyname;
  char key;

  while(1) {
    while(SDL_PollEvent(&event) && current) {

      if(event.type == SDL_KEYDOWN) {
	keyname = SDL_GetKeyName(event.key.keysym.sym);
	key = 0;
	switch(event.key.keysym.sym) {
	case SDLK_SPACE: key = ' '; break;
	case SDLK_ESCAPE: key = 27; break;
	case SDLK_RETURN: key = 13; break;
	default:
	  if(keyname[1] == 0) key = keyname[0];
	  break;
	}
	/* check: is that translation necessary? */
	if(key) 
	  current->keyboard(key, 0, 0);
	else
	  current->keyboard(event.key.keysym.sym, 0, 0);
      }
    }
    if(redisplay) {
      current->display();
      redisplay = 0;
    } else
      current->idle();
  }
}
  
void SystemRegisterCallbacks(callbacks *cb) {
  current = cb;
}

void SystemInitWindow(int x, int y, int w, int h) {
  width = w;
  height = h;
}

void SystemInitDisplayMode(int f, unsigned char full) {
  flags = f;
  fullscreen = full;
  if(flags & SYSTEM_DOUBLE)
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
  if(flags & SYSTEM_DEPTH)
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
}

int SystemCreateWindow(char *name) {
  int f = SDL_OPENGL;
  if(fullscreen & SYSTEM_FULLSCREEN)
    f |= SDL_FULLSCREEN;
  if( (screen = SDL_SetVideoMode( width, height, 0, f )) == NULL ) {
    fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
    exit(1);
  }
  else return 1;
}

void SystemDestroyWindow(int id) {
}

void SystemReshapeFunc(void(*reshape)(int, int)) {
}

extern char* SystemGetKeyName(int key) {
  /*
  char *buf;
  buf = malloc(2);
  buf[0] = (char)key;
  buf[1] = 0;
  return buf;
  */
  return SDL_GetKeyName(key);
}  




