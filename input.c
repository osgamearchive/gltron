#define KEYBOARD
#include "gltron.h"

/* I hear people are reading this file because they couldn't find the
   manual! Go to http://www.ards.net/Andreas/gltron.html#usage         */

/* default actions */

keyAction key_actions[KEY_ACTIONS_N] = {
  { 0, TURN_LEFT, 'a' },
  { 0, TURN_RIGHT, 's' },
  { 1, TURN_LEFT, 'k' },
  { 1, TURN_RIGHT, 'l' },
  { 2, TURN_LEFT, '5' },
  { 2, TURN_RIGHT, '6' },
  { 3, TURN_LEFT, SYSTEM_KEY_LEFT },
  { 3, TURN_RIGHT, SYSTEM_KEY_RIGHT }
};

#define KEY_RESERVED_N 8
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

void keyGame(int k, int x, int y)
{
  int i;

  switch (k) {
    /* case 'q': SystemExit(); break; */
  case 27:
    switchCallbacks(&pauseCallbacks);
    switchCallbacks(&guiCallbacks);
    break;
  case ' ': switchCallbacks(&pauseCallbacks); break;

  case SYSTEM_KEY_F1: defaultDisplay(0); break;
  case SYSTEM_KEY_F2: defaultDisplay(1); break;
  case SYSTEM_KEY_F3: defaultDisplay(2); break;
  case SYSTEM_KEY_F4: defaultDisplay(3); break;

  case SYSTEM_KEY_F10:
    game->settings->camType = (game->settings->camType + 1) % CAM_COUNT;
    for(i = 0; i < game->players; i++)
      if(game->player[i].ai->active == 0)
	initCamera(game->player[i].camera, 
		   game->player[i].data, 
		   game->settings->camType);
    break;

  case SYSTEM_KEY_F5: saveSettings(); break;
  case SYSTEM_KEY_F11: doBmpScreenShot(); break;
  case SYSTEM_KEY_F12: doScreenShot(); break;

  case SYSTEM_KEY_UP: consoleScrollBackward(1); break;
  case SYSTEM_KEY_DOWN: consoleScrollForward(1); break;
    /* toggle lighting
  case SYSTEM_KEY_F6: 
      game->settings->light_cycles = !game->settings->light_cycles;
      break;
    */

  default: 
    for(i = 0; i < KEY_ACTIONS_N; i++) {
      if(k == key_actions[i].key) {
	int p;
	p = key_actions[i].player;
	if(PLAYER_IS_ACTIVE(&game->player[p]))
	  createTurnEvent(p, key_actions[i].turn);
	return;
      }
    }

    sprintf(messages, "key '%s' is not bound", SystemGetKeyName(k));
    /* consoleAddLine(messages); */
    fprintf(stderr, "%s\n", messages);
  }
}

void parse_args(int argc, char *argv[]) {
  int i;
  while(argc--) {
    if(argv[argc][0] == '-') {
      i = 0;
      while(argv[argc][++i] != 0) {
	switch(argv[argc][i]) {
	case 'm': game->settings->show_model = 0; break;
	case 'x': game->settings->show_crash_texture = 0; break;
	case 'F': game->settings->show_fps = 0; break;
	case 't': game->settings->show_floor_texture = 0; break;
	case 'c': game->settings->show_ai_status = 0; break;
	case 'g': game->settings->show_glow = 0; break;
	case 'w': game->settings->show_wall = 0; break;
	case 'C': game->settings->show_ai_status = 1; break;
	case 'v': game->settings->screenSaver = 1; break;
	case 'i': game->settings->windowMode = 1; break;
	case 'O': game->settings->softwareRendering = 1; break;
	case '1': /* default is 4 */
	  game->settings->width = 320;
	  game->settings->height = 240;
	  break;
	case '2': 
	  game->settings->width = 400;
	  game->settings->height = 300;
	  break;
	case '3': 
	  game->settings->width = 512;
	  game->settings->height = 384;
	  break;
	case '4': 
	  game->settings->width = 640;
	  game->settings->height = 480;
	  break;
	case '5': 
	  game->settings->width = 800;
	  game->settings->height = 600;
	  break;
	case '6':
	  game->settings->width = 1024;
	  game->settings->height = 768;
	case '7': 
	  game->settings->width = 1280;
	  game->settings->height = 1024;
	case 's':
	  game->settings->playMusic = 0;
	  game->settings->playEffects = 0;
	  break;
	case 'h':
	default:
	  printf("Usage: %s [-FftwbghcCsk1234simo]\n\n", argv[0]);
	  printf("Options:\n\n");
	  printf("-F\tdon't display FPS counter\n");
	  printf("-t\tdon't display floor texture, use lines instead"
		 "(huge speed gain)\n");
	  printf("-w\tdon't display walls (speed gain)\n");
	  printf("-m\tdon't show lightcycle (speed gain)\n");
	  printf("-x\tdon't show crash texture (speed gain)\n");
	  printf("-g\tdon't show glows (small speed gain)\n");
	  printf("-c\tdon't show ai status\n");
	  printf("-C\tshow ai status (default: on)\n");
	  printf("-1\tSet resolution to 320x240\n");
	  printf("-2\tSet resolution to 400x300\n");
	  printf("-3\tSet resolution to 512x384\n");
	  printf("-4\tSet resolution to 640x480 (default)\n");
	  printf("-5\tSet resolution to 800x600\n");
	  printf("-6\tSet resolution to 1024x768\n");
	  printf("-7\tSet resolution to 1280x1024\n");

	  printf("-s\tDon't play sound\n");
	  /* printf("-v\tStart in demo/screensaver mode\n"); */
	  printf("-i\tforce startup in a window\n");
	  printf("-O\tenable software rendering optimization (only if "
		 "you don't have a 3d card!)\n");
	  printf("-h\tthis help\n");
	  exit(1);
	}
      }
    }
  }
}
