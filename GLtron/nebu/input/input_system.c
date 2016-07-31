#include "input/nebu_input_system.h"
#include "base/nebu_system.h"
#include "video/nebu_video_system.h"
#include "scripting/nebu_scripting.h"

#include "base/nebu_system.h"
#include <errno.h>

#include "base/nebu_debug_memory.h"

static float joystick_threshold = 0;
static int mouse_x = -1;
static int mouse_y = -1;

enum { eMaxKeyState = SDL_NUM_SCANCODES };
static int keyState[eMaxKeyState];

static void setKeyState(int scanCode, int state)
{
	if(scanCode < eMaxKeyState)
		keyState[scanCode] = state;
}

void nebu_Input_Init(void) {
	int i;

	/* keyboard */
	// SDL_EnableKeyRepeat(0, 0); /* turn keyrepeat off */
    // not needed anymore (not present in SDL2 either),
    // since we can check event.key.repeat == 0 now
  
	/* joystick */
	if(SDL_Init(SDL_INIT_JOYSTICK) >= 0) {
		SDL_Joystick *joy;
		int joysticks = SDL_NumJoysticks();
		
		/* FIXME: why only two joysticks? */
		/* joystick, currently at most 2 */
		int max_joy=2; /* default... override by setting NEBU_MAX_JOY */
		char *NEBU_MAX_JOY=getenv("NEBU_MAX_JOY");
		
		if(NEBU_MAX_JOY)
		{
			int n;
			char *endptr;
			errno=0;
			n= (int) strtol(NEBU_MAX_JOY, &endptr, 10);
			if(n<0)
				n=0;
			if(n>4)
				n=4; /* this is the max we can handle! */
			if(!*endptr && !errno)
				max_joy=n;
		}
		
		if(joysticks > max_joy)
			joysticks = max_joy;
		
		for(i = 0; i < joysticks; i++) {
			joy = SDL_JoystickOpen(i);
		}
		if(i)
			SDL_JoystickEventState(SDL_ENABLE);
	} else {
		const char *s = SDL_GetError();
		fprintf(stderr, "[init] couldn't initialize joysticks: %s\n", s);
	}
	for(i = 0; i < eMaxKeyState; i++)
	{
		keyState[i] = NEBU_INPUT_KEYSTATE_UP;
	}
}

void nebu_Input_Grab(void) {
    nebu_Input_HidePointer();
}

void nebu_Input_Ungrab(void) {
    nebu_Input_UnhidePointer();
}

void nebu_Input_HidePointer(void) {
    // SDL_SetRelativeMouseMode(SDL_TRUE);
    nebu_Log("[nebu] hiding pointer\n");
	int result = SDL_ShowCursor(SDL_DISABLE);
    if(result < 0)
        fprintf(stderr, "[error] %s\n", SDL_GetError());
    /*
    else
        fprintf(stderr, "[cursor] hidden\n");
     */
}

void nebu_Input_UnhidePointer(void) {
    // SDL_SetRelativeMouseMode(SDL_FALSE);
    nebu_Log("[nebu] showing pointer\n");
	int result = SDL_ShowCursor(SDL_ENABLE);

    if(result < 0)
        fprintf(stderr, "[error] %s\n", SDL_GetError());
    /*
     else
        fprintf(stderr, "[cursor] displayed\n");
     */
}

void SystemMouse(int buttons, int state, int x, int y) {
#ifndef __IPHONEOS__
	if(current && current->mouse)
		current->mouse(buttons, state, x, y);
#else
    // nebu_Log("emulating joystick\n");
    if(state == 0)
        return;
    
    nebu_Log("mouse: (%d, %d)\n", x, y);
    int w, h;
    nebu_Video_GetPointDimension(&w, &h);
    float u = (float) x / (float) w;
    float v = (float) (h - y) / (float) h;
    nebu_Log("scaled to [0,1]x[0,1] with origin in the lower left: (%.2f, %.2f)\n", u, v);
    
    if(u < 0.2 && v  > 0.8)
        current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, SYSTEM_SCANCODE_ESCAPE);
    else if(u < 0.2)
        current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, SYSTEM_SCANCODE_A);
    else if(u > 0.8)
        current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, SYSTEM_SCANCODE_S);
    else if(v < 0.2)
        current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, SYSTEM_SCANCODE_DOWN);
    else if(v > 0.8)
        current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, SYSTEM_SCANCODE_DOWN);
    else
        current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, SYSTEM_SCANCODE_RETURN);
#endif

}


int nebu_Input_GetKeyState(int key)
{
	if(key > eMaxKeyState)
		return NEBU_INPUT_KEYSTATE_UP;
	else
		return keyState[key];
}

void nebu_Input_Mouse_GetDelta(int *x, int *y)
{
	int wx, wy;

	if(mouse_x == -1 || mouse_y == -1)
	{
		// mouse coordinates not yet initialized
		*x = 0;
		*y = 0;
	}
	else
    {
        nebu_Video_GetPointDimension(&wx, &wy);
        *x = mouse_x - wx / 2;
        *y = mouse_y - wy / 2;
    }

	// printf("[input] returned delta %d,%d\n", *x, *y);
}

void nebu_Input_Mouse_WarpToOrigin(void)
{
	int wx, wy;
	nebu_Video_GetPointDimension(&wx, &wy);
	nebu_Video_WarpPointer(wx / 2, wy / 2);
	// printf("[input] warped to %d,%d\n", wx / 2, wy /2);
}

void SystemMouseMotion(int x, int y) {
	// save mouse position
	// printf("[input] mouse motion to %d, %d\n", x, y);
	mouse_x = x;
	mouse_y = y;
	if(current && current->mouseMotion)
		current->mouseMotion(x, y);
}

const char* nebu_Input_GetKeyNameFromScanCode(int scanCode) {
    const char *name =  SDL_GetKeyName(SDL_GetKeyFromScancode(scanCode));
    return (name[0] != 0) ? name : "unknown custom key";
}

const char* nebu_Input_GetScanNameFromScanCode(int scanCode) {
    const char *name =  SDL_GetScancodeName(scanCode);
    return (name[0] != 0) ? name : "unknown custom key";
}

int nebu_Input_GetScanCodeFromScanName(const char *name)
{
    return SDL_GetScancodeFromName(name);
}

void nebu_Intern_HandleInput(SDL_Event *event) {
    int state;

#if 0
    // int skip_axis_event = 0;
	static int joy_axis_state[2] = { 0, 0 };
	static int joy_lastaxis[2] = { 0, 0 };
#endif
    
	switch(event->type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
        // ignore key repeats
        if(event->key.repeat != 0)
            break;
            
		if(event->type == SDL_KEYDOWN) {
			state = NEBU_INPUT_KEYSTATE_DOWN;
		} else {
			state = NEBU_INPUT_KEYSTATE_UP;
		}
 
		/* check: is that translation necessary? */
		setKeyState(event->key.keysym.scancode, state);
		if(current && current->keyboard)
			current->keyboard(state, event->key.keysym.scancode);
		break;
#if 0
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
				setKeyState(key, NEBU_INPUT_KEYSTATE_UP);
				if(current && current->keyboard)
					current->keyboard(NEBU_INPUT_KEYSTATE_UP, key, 0, 0);
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
				setKeyState(key, NEBU_INPUT_KEYSTATE_DOWN);
				if(current && current->keyboard)
					current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, key, 0, 0);
			} else {
				// do nothing
			}
		}
		break;
#endif
				 
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
		setKeyState(key, NEBU_INPUT_KEYSTATE_DOWN);
		if(current && current->keyboard)
			current->keyboard(NEBU_INPUT_KEYSTATE_DOWN, key, 0, 0);
		break;
#endif
            
#if 0
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		if(event->type == SDL_JOYBUTTONDOWN)
			state = NEBU_INPUT_KEYSTATE_DOWN;
		else
			state = NEBU_INPUT_KEYSTATE_UP;
		
		key = SYSTEM_JOY_BUTTON_0 + event->jbutton.button +
			SYSTEM_JOY_OFFSET * event->jbutton.which;
		setKeyState(key, state);
		if(current && current->keyboard)
			current->keyboard(state, key, 0, 0);
		break;
#endif
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
