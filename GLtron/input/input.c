#include "video/video.h"
#include "game/game.h"
#include "game/camera.h"
#include "game/event.h"
#include "configuration/settings.h"
#include "scripting/scripting.h"

#include "base/nebu_math.h"

#include "Nebu_input.h"
#include "Nebu_scripting.h"
#include "input/input.h"
#include "video/nebu_console.h"

#include "base/nebu_debug_memory.h"

int ReservedScanCodes[eReservedKeys] = {
  SYSTEM_SCANCODE_ESCAPE,
  SYSTEM_SCANCODE_SPACE,
  SYSTEM_SCANCODE_F1,
  SYSTEM_SCANCODE_F2,
  SYSTEM_SCANCODE_F3,
  SYSTEM_SCANCODE_F4,
  SYSTEM_SCANCODE_F5,
  SYSTEM_SCANCODE_F6,
  SYSTEM_SCANCODE_F7,
  SYSTEM_SCANCODE_F8,
  SYSTEM_SCANCODE_F9,
  SYSTEM_SCANCODE_F10,
  SYSTEM_SCANCODE_F11,
  SYSTEM_SCANCODE_F12,
};


int getScanCodeForAction(int player, char *actionName)
{
    char *keyName;
    scripting_RunFormat("return settings.keys[%d].%s", player + 1, actionName);
    scripting_GetStringResult(&keyName);
    int result = nebu_Input_GetScanCodeFromScanName(keyName);
    scripting_StringResult_Free(keyName);
    return result;
}

void keyGame(int state, int scanCode)
{
	int i;

	if(state == NEBU_INPUT_KEYSTATE_DOWN)
	{
		switch (scanCode)
		{
		case SYSTEM_SCANCODE_ESCAPE:
			game->pauseflag = PAUSE_GAME_SUSPENDED;
			nebu_System_ExitLoop(eSRC_Game_Escape);
			return;
		case SYSTEM_SCANCODE_SPACE:
			game->pauseflag = PAUSE_GAME_SUSPENDED;
			nebu_System_ExitLoop(eSRC_Game_Pause);
			return;
		case SYSTEM_SCANCODE_F1: changeDisplay(0); return;
		case SYSTEM_SCANCODE_F2: changeDisplay(1); return;
		case SYSTEM_SCANCODE_F3: changeDisplay(2); return;
		case SYSTEM_SCANCODE_F4: changeDisplay(3); return;

		// somehow, this breaks the 'keys' array, and saving
		// at the end of the game fails
		// case SYSTEM_SCANCODE_F5: saveSettings(); return;

        case SYSTEM_SCANCODE_F6: toggleCameraLock(); return;
		// case SYSTEM_SCANCODE_F10: nextCameraType(); return;
        case SYSTEM_SCANCODE_F5: nextCameraType(); return;
            
        case SYSTEM_SCANCODE_F11: doBmpScreenShot(gScreen); return;
		case SYSTEM_SCANCODE_F12: doPngScreenShot(gScreen); return;

		// case SYSTEM_SCANCODE_F6: console_Seek(-1); return;
		// case SYSTEM_SCANCODE_F7: console_Seek(1); return;

		case SYSTEM_SCANCODE_F8:
			// toggle wireframe setting
			setSettingi("wireframe", getSettingi("wireframe") ? 0 : 1);
			return;

    /* toggle lighting
  case SYSTEM_SCANCODE_F6: 
      setSettingi("light_cycles", !game->settings->light_cycles);
      return;
    */
		}
	}
	// foreach player, check if the player's movement keys are pressed
	// TODO: This is ugly as else, better create a mapping for each 
	// key to each action
	for( i = 0; i < game->players; i++)
	{
		if(PLAYER_IS_ACTIVE(&game->player[i]) &&
			 !game->player[i].ai.active)
		{
			if(state == NEBU_INPUT_KEYSTATE_DOWN)
			{
				if(scanCode == getScanCodeForAction(i, "left"))
				{
					createEvent(i, EVENT_TURN_LEFT);
					return;
				}
				if(scanCode == getScanCodeForAction(i, "right"))
				{
					createEvent(i, EVENT_TURN_RIGHT);
					return;
				}
			}
			// deal with glance keys
			if(scanCode == getScanCodeForAction(i, "glance_left"))
			{
				if(state == NEBU_INPUT_KEYSTATE_DOWN)
				{
					// nebu_Log("glance left down\n");
					gppPlayerVisuals[i]->camera.bIsGlancing = 1;
				}
				else
				{
					// nebu_Log("glance left up\n");
					gppPlayerVisuals[i]->camera.bIsGlancing = 0;
				}
				return;
			}
			// deal with glance keys
            if(scanCode == getScanCodeForAction(i, "glance_right"))
			{
				if(state == NEBU_INPUT_KEYSTATE_DOWN)
				{
					// nebu_Log("glance right down\n");
					gppPlayerVisuals[i]->camera.bIsGlancing = -1;
				} else {
					// nebu_Log("glance right up\n");
					gppPlayerVisuals[i]->camera.bIsGlancing = 0;
				}
				return;
			}
			// boost
            if(scanCode == getScanCodeForAction(i, "boost"))
			{
				if(state == NEBU_INPUT_KEYSTATE_DOWN)
				{
					// nebu_Log("boost down\n");
					if(getSettingi("booster_on") &&
						game->player[i].data.energy > getSettingf("booster_min"))
					{
						game->player[i].data.boost_enabled = 1;
					}
				}
				else
				{
					// nebu_Log("boost up\n");
					game->player[i].data.boost_enabled = 0;
				}
				return;
			}
			// wallbuster
            if(scanCode == getScanCodeForAction(i, "bust"))
			{
				if(state == NEBU_INPUT_KEYSTATE_DOWN)
				{
					// nebu_Log("wall_buster down\n");
					if(getSettingi("wall_buster_on") &&
						game->player[i].data.energy > getSettingf("wall_buster_min"))
					{
						game->player[i].data.wall_buster_enabled = 1;
					}
				}
				else
				{
					// nebu_Log("wall_buster up\n");
					game->player[i].data.wall_buster_enabled = 0;
				}
				return;
			}
		}
	}
	if(state == NEBU_INPUT_KEYSTATE_DOWN) {
		displayMessage(TO_STDERR, "key '%s' (%d) is not bound", 
									 nebu_Input_GetKeyNameFromScanCode(scanCode), scanCode);
	}
}

void parse_args(int argc, const char *argv[]) {
  int i;
  while(argc--) {
    if(argv[argc][0] == '-') {
      i = 0;
      while(argv[argc][++i] != 0) {
				switch(argv[argc][i]) {
				case 'x': setSettingi("show_crash_texture", 0); break;
				case 'F': setSettingi("show_fps", 0); break;
				case 't': setSettingi("show_floor_texture", 0); break;
				case 'c': setSettingi("show_ai_status", 0); break;
				case 'g': setSettingi("show_glow", 0); break;
				case 'w': setSettingi("show_wall", 0); break;
				case 'C': setSettingi("show_ai_status", 1); break;
				case 'i': setSettingi("windowMode", 1); break;
				case 'O': setSettingi("softwareRendering", 1); break;
				case '1': /* default is 4 */
					setSettingi("width", 320);
					setSettingi("height", 240);
					break;
				case '2': 
					setSettingi("width", 512);
					setSettingi("height", 384);
					break;
				case '3': 
					setSettingi("width", 640);
					setSettingi("height", 480);
					break;
				case '4': 
					setSettingi("width", 800);
					setSettingi("height", 600);
					break;
				case '5':
					setSettingi("width", 1024);
					setSettingi("height", 768);
					break;
				case '6': 
					setSettingi("width", 1280);
					setSettingi("height", 1024);
					break;
				case '7': 
					setSettingi("width", 1600);
					setSettingi("height", 1200);
					break;
				case 's':
					setSettingi("playMusic", 0);
					setSettingi("playEffects", 0);
					break;
				case 'h':
				default:
					printf("Usage: %s [-FftwbghcCsk1234simo]\n\n", argv[0]);
					printf("Options:\n\n");
					printf("-F\tdon't display FPS counter\n");
					printf("-t\tdon't display floor texture, use lines instead"
								 "(huge speed gain)\n");
					printf("-w\tdon't display walls (speed gain)\n");
					printf("-x\tdon't show crash texture (speed gain)\n");
					printf("-g\tdon't show glows (small speed gain)\n");
					printf("-c\tdon't show ai status\n");
					printf("-C\tshow ai status (default: on)\n");
																				
					printf("-1\tSet resolution to 320x240\n");
					printf("-2\tSet resolution to 512x384\n");
					printf("-3\tSet resolution to 640x480\n");
					printf("-4\tSet resolution to 800x600 (default)\n");
					printf("-5\tSet resolution to 1024x768\n");
					printf("-6\tSet resolution to 1280x1024\n");
					printf("-7\tSet resolution to 1600x1200\n");

					printf("-s\tDon't play sound\n");
					/* printf("-v\tStart in demo/screensaver mode\n"); */
					printf("-i\tforce startup in a window\n");
					printf("-O\tenable software rendering optimization (only if "
								 "you don't have a 3d card!)\n");
					printf("-h\tthis help\n");
					exit(0); /* OK: end of program */
				}
      }
    }
  }
}

void Input_Idle(void) {
	SystemSetJoyThreshold( getSettingf("joy_threshold") );
}
