#include "input/nebu_input_system.h"
#include "input/nebu_system_keynames.h"
#include "base/nebu_system.h"
#include "scripting/nebu_scripting.h"

#include "SDL.h"

static float joystick_threshold = 0;

void SystemGrabInput() {
  SDL_WM_GrabInput(SDL_GRAB_ON);
}

void SystemUngrabInput() {
  SDL_WM_GrabInput(SDL_GRAB_OFF);
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

void SystemHandleInput(SDL_Event *event) {
  char *keyname;
  int key;
  int skip_axis_event = 0;

	switch(event->type) {
	case SDL_KEYDOWN:
		keyname = SDL_GetKeyName(event->key.keysym.sym);
		key = 0;
		switch(event->key.keysym.sym) {
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
			current->keyboard(event->key.keysym.sym, 0, 0);
		break;
	case SDL_JOYAXISMOTION:
		if (abs(event->jaxis.value) <= joystick_threshold * SYSTEM_JOY_AXIS_MAX) {
			skip_axis_event &= ~(1 << event->jaxis.axis);
			break;
		}
		if(skip_axis_event & (1 << event->jaxis.axis))
			break;
		skip_axis_event |= 1 << event->jaxis.axis;
		key = SYSTEM_JOY_LEFT + event->jaxis.which * SYSTEM_JOY_OFFSET;
		if(event->jaxis.axis == 1)
			key += 2;
		if(event->jaxis.value > 0)
			key++;
		current->keyboard(key, 0, 0);
		break;
	case SDL_JOYBUTTONDOWN:
		key = SYSTEM_JOY_BUTTON_0 + event->jbutton.button +
			SYSTEM_JOY_OFFSET * event->jbutton.which;
		current->keyboard(key, 0, 0);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		SystemMouse(event->button.button, event->button.state, 
								event->button.x, event->button.y);
		break;
	case SDL_MOUSEMOTION:
		SystemMouseMotion(event->motion.x, event->motion.y);
		break;
	}
}

void SystemSetJoyThreshold(float f) { 
	joystick_threshold = f;
}