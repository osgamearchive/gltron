#include "input/nebu_input_system.h"
#include "input/nebu_system_keynames.h"
#include "base/nebu_system.h"
#include "scripting/nebu_scripting.h"

#include "SDL.h"
#include <stdlib.h>

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
  int key, state;
  // int skip_axis_event = 0;
	static int joy_axis_state[2] = { 0, 0 };
	static int joy_lastaxis[2] = { 0, 0 };

	switch(event->type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		if(event->type == SDL_KEYDOWN) {
			state = SYSTEM_KEYSTATE_DOWN;
		} else {
			state = SYSTEM_KEYSTATE_UP;
		}
 
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
			current->keyboard(state, key, 0, 0);
		else
			current->keyboard(state, event->key.keysym.sym, 0, 0);
		break;
	case SDL_JOYAXISMOTION:
		if( abs(event->jaxis.value) <= joystick_threshold * SYSTEM_JOY_AXIS_MAX) {
			// axis returned to origin, only generate event if it was set before
			if(joy_axis_state[event->jaxis.which] & (1 << event->jaxis.axis)) {
				joy_axis_state[event->jaxis.which] &= ~ 
					(1 << event->jaxis.axis); // clear axis
				key = SYSTEM_JOY_LEFT + event->jaxis.which * SYSTEM_JOY_OFFSET;
				if(event->jaxis.axis == 1) {
					key += 2;
				}
				if(joy_lastaxis[event->jaxis.which] & (1 << event->jaxis.axis)) {
					key++;
				}
				current->keyboard(SYSTEM_KEYSTATE_UP, key, 0, 0);
			} else {
				// do nothing
			}
		} else {
			// axis set, only generate event if it wasn't set before
			if(! (joy_axis_state[event->jaxis.which] & (1 << event->jaxis.axis)) ) {
				joy_axis_state[event->jaxis.which] |= (1 << event->jaxis.axis);
				key = SYSTEM_JOY_LEFT + event->jaxis.which * SYSTEM_JOY_OFFSET;
				if(event->jaxis.axis == 1) {
					key += 2;
				}
				if(event->jaxis.value > 0) {
					key++;
					joy_lastaxis[event->jaxis.which] |= (1 << event->jaxis.axis);
				} else {
					joy_lastaxis[event->jaxis.which] &= ~(1 << event->jaxis.axis);
				}
				current->keyboard(SYSTEM_KEYSTATE_DOWN, key, 0, 0);
			} else {
				// do nothing
			}
		}
		break;
				 
#if 0
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
		current->keyboard(SYSTEM_KEYSTATE_DOWN, key, 0, 0);
		break;
#endif
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		if(event->type == SDL_JOYBUTTONDOWN)
			state = SYSTEM_KEYSTATE_DOWN;
		else
			state = SYSTEM_KEYSTATE_UP;
		
		key = SYSTEM_JOY_BUTTON_0 + event->jbutton.button +
			SYSTEM_JOY_OFFSET * event->jbutton.which;
		current->keyboard(state, key, 0, 0);
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
