/* system specific functions (basically, an SDL wrapper) */

#include "system.h"
#include "system_keynames.h"

static int redisplay = 0;
static callbacks *current = 0;
static int return_code = -1;
static SDL_Surface *screen;
static int width, height;
static int flags;
static int fullscreen;
static int video_initialized = 0;

void SystemExit() {
  fprintf(stderr, "[system] shutting down sound now\n");
  Sound_shutdown();
  fprintf(stderr, "[system] shutting down network now\n");
#ifdef NETWORK
  SystemNetExit();
#endif
  fprintf(stderr, "[system] shutting down sdl now\n");
  SDL_Quit();
  fprintf(stderr, "[system] exiting application\n");
  exit(0); /* OK: end of program */
}

void SystemInit(int *argc, char *argv[]) {
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL video: %s\n", SDL_GetError());
    exit(1); /* OK: critical, no visual */
  }
  else video_initialized = 1;

  if(SDL_Init(SDL_INIT_AUDIO) < 0 ){
    fprintf(stderr, "Couldn't initialize SDL audio: %s\n", SDL_GetError());
    /* FIXME: disable sound system */
  }
	/* keyboard */
  SDL_EnableKeyRepeat(0, 0); /* turn keyrepeat off */
	
	{
		int i;
		SDL_Joystick *joy;
		int joysticks = SDL_NumJoysticks();
		
		/* joystick, currently at most 2 */
		if(joysticks > 2)
			joysticks = 2;
		
		for(i = 0; i < joysticks; i++) {
			joy = SDL_JoystickOpen(i);
		}
		if(i)
			SDL_JoystickEventState(SDL_ENABLE);
	}
			
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

unsigned int SystemGetElapsedTime() {
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

int SystemMainLoop() {
  SDL_Event event;
  char *keyname;
  int key;
  int skip_axis_event = 0;
  
	return_code = -1;
  while(return_code == -1) {
    while(SDL_PollEvent(&event) && current) {
			switch(event.type) {
			case SDL_KEYDOWN:
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
				break;
			case SDL_JOYAXISMOTION:
				if (abs(event.jaxis.value) <= (getSettingf("joy_threshold") * SYSTEM_JOY_AXIS_MAX)) {
					skip_axis_event &= ~(1 << event.jaxis.axis);
					break;
				}
				if(skip_axis_event & (1 << event.jaxis.axis))
					break;
				skip_axis_event |= 1 << event.jaxis.axis;
				key = SYSTEM_JOY_LEFT + event.jaxis.which * SYSTEM_JOY_OFFSET;
				if(event.jaxis.axis == 1)
					key += 2;
				if(event.jaxis.value > 0)
					key++;
				current->keyboard(key, 0, 0);
				break;
			case SDL_JOYBUTTONDOWN:
				key = SYSTEM_JOY_BUTTON_0 + event.jbutton.button +
					SYSTEM_JOY_OFFSET * event.jbutton.which;
				current->keyboard(key, 0, 0);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				SystemMouse(event.button.button, event.button.state, 
										event.button.x, event.button.y);
				break;
			case SDL_MOUSEMOTION:
				SystemMouseMotion(event.motion.x, event.motion.y);
				break;
			case SDL_QUIT:
        SystemQuit ();
				break;
			default:
				/* ignore event */
				break;
      }
    }
    if(redisplay) {
      current->display();
      redisplay = 0;
    } else
      current->idle();
  }
	if(current->exit)
		(current->exit)();
	return return_code;
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
	SDL_WM_SetCaption("GLtron", "");
  return 1;
}

void SystemDestroyWindow(int id) {
  /* quit the video subsytem
	 * otherwise SDL can't create a new context on win32, if the stencil
	 * bits change 
	 */
	/* there used to be some problems (memory leaks, unprober driver unloading)
	 * caused by this, but I can't remember what they where
	 */
		 
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  video_initialized = 0;
}

void SystemReshapeFunc(void(*reshape)(int, int)) {
}

extern char* SystemGetKeyName(int key) {
	if(key < SYSTEM_CUSTOM_KEYS)
		return SDL_GetKeyName(key);
	else {
		int i;
		
		for(i = 0; i < CUSTOM_KEY_COUNT; i++) {
			if(custom_keys.key[i].key == key)
				return custom_keys.key[i].name;
		}
		return "unknown custom key";
	}
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
  static int quitting = 0;
  if(quitting) {
		SystemExitLoop(RETURN_QUIT);
    // SystemExit();
  } else {
    saveSettings(); 
		SystemExitLoop(RETURN_CREDITS);
    quitting = 1;
  }
}

void SystemExitLoop(int value) {
	return_code = value;
}
