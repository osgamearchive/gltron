#include "audio/audio.h"
#include "video/graphics_utility.h"
#include "video/video.h"
#include "game/game.h"
#include "game/game_data.h"
#include "input/input.h"
#include "video/nebu_renderer_gl.h"
#include "configuration/configuration.h"
#include "configuration/settings.h"
#include "video/nebu_video_system.h"
#include "input/nebu_input_system.h"

#include "scripting/nebu_scripting.h"

/* FIXME: "ignored playMenuFX" */
void playMenuFX(int foo) { }

void drawGuiBackground(void) {
  nebu_Video_CheckErrors("gui background start");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // rasonly(gScreen);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,gpGUIBackground->w,0,gpGUIBackground->h,0,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  nebu_2d_Draw(gpGUIBackground);
}
  
void displayGui(void) {
  drawGuiBackground();
  drawMenu(gScreen);

  nebu_System_SwapBuffers();  
}

void displayConfigure(void) {
  char message[] = "Press a key for this action!";
  drawGuiBackground();
  drawMenu(gScreen);

  rasonly(gScreen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, gScreen->vp_w / 6.0f, 20,
	   gScreen->vp_w / (6.0f / 4.0f * strlen(message)), message);
  nebu_System_SwapBuffers();
}

void idleGui(void) {
	Sound_idle();
	scripting_RunGC();
	Video_Idle();
	Input_Idle();
	nebu_Time_FrameDelay(50);
	nebu_System_PostRedisplay(); /* animate menu */
}

void keyboardConfigure(int state, int key, int x, int y) {
	if(state == SYSTEM_KEYSTATE_DOWN) {
		if(key != 27) /* don't allow escape */
		{
			int i;
			int isReserved = 0;
			for(i = 0; i < eReservedKeys; i++)
			{
				if(key == ReservedKeyCodes[i])
				{
					isReserved = 1;
					break;
				}
			}
			if(!isReserved)
			{
				scripting_RunFormat("settings.keys[ configure_player ]"
													"[ configure_event ] = %d", key);
			}
		}
		nebu_System_ExitLoop(RETURN_PROMPT_ESCAPE);
	}
}

void keyboardGui(int state, int key, int x, int y) {
  char *pMenuName;

	if(state == SYSTEM_KEYSTATE_UP)
		return;

  scripting_Run("return Menu.current");
  scripting_GetStringResult(&pMenuName);

  switch(key) {
  case 27:
    if(strcmp(pMenuName, "RootMenu")) {
			// not in the root menu -> go up one menu
      scripting_Run("MenuFunctions.GotoParent()");
    } else {
			// is a game already in process? then resume
			if(game->pauseflag != PAUSE_NO_GAME)
				nebu_System_ExitLoop(RETURN_GUI_ESCAPE);
    }
    break;
  case ' ': 
  case SYSTEM_KEY_RETURN:
	case SYSTEM_JOY_BUTTON_0:
	case SYSTEM_JOY_BUTTON_1:
	case SYSTEM_JOY_BUTTON_2:
	case SYSTEM_JOY_BUTTON_3:
	case SYSTEM_JOY_BUTTON_4:
	case SYSTEM_JOY_BUTTON_5:
	case SYSTEM_JOY_BUTTON_6:
	case SYSTEM_JOY_BUTTON_7:
	case SYSTEM_JOY_BUTTON_8:
	case SYSTEM_JOY_BUTTON_9:
	case SYSTEM_JOY_BUTTON_0 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_1 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_2 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_3 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_4 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_5 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_6 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_7 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_8 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_9 + SYSTEM_JOY_OFFSET:
    scripting_Run("MenuFunctions.Action()");
    break;
  case SYSTEM_KEY_UP:
	case SYSTEM_JOY_UP:
	case SYSTEM_JOY_UP + SYSTEM_JOY_OFFSET:
    scripting_Run("MenuFunctions.Previous()");
    break;
  case SYSTEM_KEY_DOWN:
	case SYSTEM_JOY_DOWN:
	case SYSTEM_JOY_DOWN + SYSTEM_JOY_OFFSET:
    scripting_Run("MenuFunctions.Next()");
    break;
  case SYSTEM_KEY_RIGHT:
	case SYSTEM_JOY_RIGHT:
		case SYSTEM_JOY_RIGHT + SYSTEM_JOY_OFFSET:
    scripting_Run("MenuFunctions.Right()");
    break;
  case SYSTEM_KEY_LEFT:
	case SYSTEM_JOY_LEFT:
		case SYSTEM_JOY_LEFT + SYSTEM_JOY_OFFSET:
    scripting_Run("MenuFunctions.Left()");
    break;
  case SYSTEM_KEY_F11: doBmpScreenShot(gScreen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(gScreen); break;
  default: 
    // printf("got key %d\n", key);
		break;
  }
  free(pMenuName);
}

void initGui(void) {
  nebu_Input_UnhidePointer();

  updateSettingsCache();
}

void guiMouse(int buttons, int state, int x, int y) {
  fprintf(stderr, "Mouse buttons: %d, State %d, Position (%d, %d)\n",
	  buttons, state, x, y); 

  /* fprintf(stderr, "testing for state == %d\n", SYSTEM_MOUSEPRESSED); */
  if (state == SYSTEM_MOUSEPRESSED) {	
    if(getSettingi("playEffects"))
      playMenuFX(fx_action);
  }
}

void guiMouseMotion(int mx, int my) {
  /* TODO: add mouse cursor, highlighted areas, etc. */
}

Callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, NULL /* exit */,
  NULL /* mouse button */, NULL /* mouse motion */, "configure"
};

Callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, NULL /* exit */,
  guiMouse, guiMouseMotion, "gui"
};

