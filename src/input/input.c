#include "video/video.h"
#include "game/game.h"

#include "Nebu_input.h"
#include "Nebu_scripting.h"

/* I hear people are reading this file because they couldn't find the
   manual! Go to http://www.gltron.org/ */

int reserved_keys[] = {
  'q', 27, ' ',
  SYSTEM_KEY_F1,
  SYSTEM_KEY_F2,
  SYSTEM_KEY_F3,
  SYSTEM_KEY_F4,
  SYSTEM_KEY_F5,
  SYSTEM_KEY_F10,
  SYSTEM_KEY_F11,
  SYSTEM_KEY_F12,
  SYSTEM_KEY_UP,
  SYSTEM_KEY_DOWN
};

void keyGame(int state, int k, int x, int y)
{
  int i;

	if(state == SYSTEM_KEYSTATE_DOWN) {
		switch (k) {
			/* case 'q': SystemExit(); return; */
		case 27:
			SystemExitLoop(RETURN_GAME_ESCAPE);
			return;
		case ' ':
			SystemExitLoop(RETURN_GAME_PAUSE);
			return;
		case SYSTEM_KEY_F1: changeDisplay(0); return;
		case SYSTEM_KEY_F2: changeDisplay(1); return;
		case SYSTEM_KEY_F3: changeDisplay(2); return;
		case SYSTEM_KEY_F4: changeDisplay(3); return;

		case SYSTEM_KEY_F5: saveSettings(); return;

		case SYSTEM_KEY_F10: nextCameraType(); return;
		case SYSTEM_KEY_F11: doBmpScreenShot(gScreen); return;
		case SYSTEM_KEY_F12: doPngScreenShot(gScreen); return;

		case SYSTEM_KEY_UP: consoleScrollBackward(1); return;
		case SYSTEM_KEY_DOWN: consoleScrollForward(1); return;
    /* toggle lighting
  case SYSTEM_KEY_F6: 
      setSettingi("light_cycles", !game->settings->light_cycles);
      return;
    */
		}
	}
	for( i = 0; i < game->players; i++) {
		if(PLAYER_IS_ACTIVE(&game->player[i]) &&
			 !game->player[i].ai->active) {
			int key;
			if(state == SYSTEM_KEYSTATE_DOWN) { 
				scripting_RunFormat("return settings.keys[%d].left", i + 1);
				scripting_GetIntegerResult( &key );
				if(key == k) {
					createEvent(i, EVENT_TURN_LEFT);
					return;
				}
				scripting_RunFormat("return settings.keys[%d].right", i + 1);
				scripting_GetIntegerResult( &key );
				if(key == k) {
					createEvent(i, EVENT_TURN_RIGHT);
					return;
				}
			}
			// deal with glance keys
			scripting_RunFormat("return settings.keys[%d].glance_left", i + 1);
			scripting_GetIntegerResult( &key );
			if(key == k) {
				if(state == SYSTEM_KEYSTATE_DOWN) {
					printf("glance left down\n");
					game->player[i].camera->movement[CAM_PHI_OFFSET] = PI / 2.0f;
				}	else {
					printf("glance left up\n");
					game->player[i].camera->movement[CAM_PHI_OFFSET] = 0;
				}
				return;
			}
			// deal with glance keys
			scripting_RunFormat("return settings.keys[%d].glance_right", i + 1);
			scripting_GetIntegerResult( &key );
			if(key == k) {
				if(state == SYSTEM_KEYSTATE_DOWN) {
					printf("glance right down\n");
					game->player[i].camera->movement[CAM_PHI_OFFSET] = - PI / 2.0f;
				} else {
					printf("glance right up\n");
					game->player[i].camera->movement[CAM_PHI_OFFSET] = 0;
				}
				return;
			}
			// boost
			scripting_RunFormat("return settings.keys[%d].boost", i + 1);
			scripting_GetIntegerResult( &key );
			if(key == k) {
				if(state == SYSTEM_KEYSTATE_DOWN) {
					printf("boost down\n");
					if(game->player[i].data->booster > getSettingf("booster_min"))
						game->player[i].data->boost_enabled = 1;
				} else {
					printf("boost up\n");
					game->player[i].data->boost_enabled = 0;
				}
				return;
			}
		}
	}
	if(state == SYSTEM_KEYSTATE_DOWN) {
		displayMessage(TO_STDERR, "key '%s' (%d) is not bound", 
									 SystemGetKeyName(k), k);
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
