/* system specific functions (basically, an SDL wrapper) */

#include "system.h"

static int redisplay = 0;
static callbacks *current = 0;
static SDL_Surface *screen;
static int width, height;
static int flags;
static int fullscreen;
static int video_initialized = 0;

void SystemExit() {
  fprintf(stderr, "shutting down sound now\n");
#ifdef SOUND
  shutdownSound();
#endif
  fprintf(stderr, "shutting down network now\n");
#ifdef NETWORK
  SystemNetExit();
#endif
  fprintf(stderr, "shutting down sdl now\n");
  SDL_Quit();
  fprintf(stderr, "exiting application\n");
  exit(0);
}

void SystemInit(int *argc, char *argv[]) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL video: %s\n", SDL_GetError());
    exit(2);
  }
  else video_initialized = 1;

  if(SDL_Init(SDL_INIT_AUDIO) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL audio: %s\n", SDL_GetError());
  }
  /* atexit(SystemExit); */

  SDL_EnableKeyRepeat(0, 0); /* turn keyrepeat off */
#ifdef NETWORK
  SystemNetInit();
#endif
}

void SystemGrabInput() {
  SDL_WM_GrabInput(SDL_GRAB_ON);
}

void SystemUngrabInput() {
  SDL_WM_GrabInput(SDL_GRAB_OFF);
}

void SystemPostRedisplay() {
  redisplay = 1;
}

int SystemGetElapsedTime() {
  /* fprintf(stderr, "%d\n", SDL_GetTicks()); */
  return SDL_GetTicks();
}

void SystemSwapBuffers() {
  SDL_GL_SwapBuffers();
}

void SystemWarpPointer(int x, int y) {
  SDL_WarpMouse(x, y);
}

void SystemHidePointer() {
  SDL_ShowCursor(SDL_DISABLE);
}

void SystemUnhidePointer() {
  SDL_ShowCursor(SDL_ENABLE);
}

void SystemMouse(int buttons, int state, int x, int y) {
  if(current)
    if(current->mouse != NULL)
      current->mouse(buttons, state, x, y);
}

void SystemMouseMotion(int x, int y) {
  if(current)
    if(current->mouseMotion != NULL)
      current->mouseMotion(x, y);
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
      } else if(event.type == SDL_MOUSEBUTTONDOWN ||
		event.type == SDL_MOUSEBUTTONUP) {
	SystemMouse(event.button.button, event.button.state, 
		    event.button.x, event.button.y);
      } else if(event.type == SDL_MOUSEMOTION) {
	SystemMouseMotion(event.motion.x, event.motion.y);
      } else if(event.type == SDL_QUIT) {
        SystemQuit ();
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
  int bitdepth, zdepth;

  flags = f;
  fullscreen = full;
  if(!video_initialized) {
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "can't initialize Video: %s\n", SDL_GetError());
      exit(2);
    }
  } else {
    fprintf(stderr, "WARNING: can't init video; it's already running\n");
  }
  if(flags & SYSTEM_DOUBLE)
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);

  if(flags & SYSTEM_32_BIT) {
    zdepth = 24;
    bitdepth = 32;
  } else {
    zdepth = 16;
    bitdepth = 16;
  }
  if(flags & SYSTEM_DEPTH)
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, zdepth);
  /* SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8); */
#ifdef DO_STENCIL
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8);
#endif
  video_initialized = 1;
#warning "bitdepth value unused"
}

void SystemSetGamma(float red, float green, float blue) {
  SDL_SetGamma(red, green, blue);
}

int SystemCreateWindow(char *name) {
  int f = SDL_OPENGL;
  if(fullscreen & SYSTEM_FULLSCREEN)
    f |= SDL_FULLSCREEN;
  if( (screen = SDL_SetVideoMode( width, height, 0, f )) == NULL ) {
    fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
    exit(1);
  }
  return 1;
}

void SystemDestroyWindow(int id) {
  /* don't quit the video subsystem, SDL fails unloading GL drivers */
  /* SDL_QuitSubSystem(SDL_INIT_VIDEO); */
  video_initialized = 0;
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

int SystemWriteBMP(char *filename, int x, int y, unsigned char *pixels) {
  /* this code is shamelessly stolen from Ray Kelm, but I believe he
     put it in the public domain */
  SDL_Surface *temp;
  int i;

  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			      0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
			      0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
			      );

  if (temp == NULL)
    return -1;

  for (i = 0; i < y; i++)
    memcpy(((char *) temp->pixels) + temp->pitch * i, 
	   pixels + 3 * x * (y - i - 1), x * 3);

  SDL_SaveBMP(temp, filename);
  SDL_FreeSurface(temp);
  return 0;
}

void SystemQuit() {
      saveSettings(); 
      switchCallbacks(&creditsCallbacks);
}
