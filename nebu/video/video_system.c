#include "video/nebu_renderer_gl.h"
#include "video/nebu_video_system.h"

#include "SDL.h"

#include <assert.h>

static SDL_Surface *screen;
static int width, height;
static int flags;
static int fullscreen;
static int video_initialized = 0;
static int window_id;

void nebu_Video_SwapBuffers() {
	SDL_GL_SwapBuffers();
}

void nebu_Video_Init(void) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Couldn't initialize SDL video: %s\n", SDL_GetError());
    exit(1); /* OK: critical, no visual */
  } else
		video_initialized = 1;
}

void SystemInitWindow(int x, int y, int w, int h) {
  fprintf(stderr, "ignoring (%d,%d) initial window position - feature not implemented\n", x, y);
  width = w;
  height = h;
}

void SystemInitDisplayMode(int f, unsigned char full) {
  int bitdepth, zdepth;

  flags = f;
  fullscreen = full;
  if(!video_initialized) {
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "[system] can't initialize Video: %s\n", SDL_GetError());
      exit(1); /* OK: critical, no visual */
    }
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
  if(flags & SYSTEM_STENCIL)
     SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8);
  else 
     SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0);
  video_initialized = 1;
  /* FIXME: bitdepth value unused */
}

void SystemSetGamma(float red, float green, float blue) {
  SDL_SetGamma(red, green, blue);
}

int SystemCreateWindow(char *name) {
  int f = SDL_OPENGL;
  if(fullscreen & SYSTEM_FULLSCREEN)
    f |= SDL_FULLSCREEN;
  if( (screen = SDL_SetVideoMode( width, height, 0, f )) == NULL ) {
    fprintf(stderr, "[system] Couldn't set GL mode: %s\n", SDL_GetError());
    exit(1); /* OK: critical, no visual */
  }
  SDL_WM_SetCaption(name, NULL);
  glewInit();
  if(!GLEW_ARB_multitexture)
  {
	  fprintf(stderr, "multitexturing is not available\n");
	  exit(1);
  }
  fprintf(stderr, "GL vendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "GL renderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "GL version: %s\n", glGetString(GL_VERSION));

  SDL_WM_SetCaption("GLtron", "");
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapBuffers();
  return 0;
}

void SystemDestroyWindow(int id) {
  /* quit the video subsytem
	 * otherwise SDL can't create a new context on win32, if the stencil
	 * bits change 
	 */
	/* there used to be some problems (memory leaks, unprober driver unloading)
	 * caused by this, but I can't remember what they where
	 */
  if(id == window_id)
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
  else
	  assert(0);
  video_initialized = 0;
}

void SystemReshapeFunc(void(*reshape)(int w, int h)) {
	fprintf(stderr, "can't set reshape function (%p) - feature not supported\n", reshape);
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

void nebu_Video_WarpPointer(int x, int y) {
  SDL_WarpMouse( (Uint16)x, (Uint16)y);
}

void nebu_Video_CheckErrors(const char *where) {
  int error;
  error = glGetError();
  if(error != GL_NO_ERROR)
    printf("[glError: %s] - %d\n", where, error);
}
