#define KEYBOARD
#include "gltron.h"

/* I hear people are reading this file because they couldn't find the
   manual! Go to http://www.ards.net/Andreas/gltron.html#usage         */

/* default actions */


keyAction key_actions[KEY_ACTIONS_N] = {
  { 0, 3, 'a' },
  { 0, 1, 's' },
  { 1, 3, 'k' },
  { 1, 1, 'l' },
  { 2, 3, '5' },
  { 2, 1, '6' },
  { 3, 3, SYSTEM_KEY_LEFT },
  { 3, 1, SYSTEM_KEY_RIGHT }
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
  SYSTEM_KEY_F12
};

void keyGame(int k, int x, int y)
{
  int i;

  switch (k) {
  case 'q': exit(0); break; /* panic button */
  case 27: switchCallbacks(&guiCallbacks); break;
  case ' ': switchCallbacks(&pauseCallbacks); break;

  case SYSTEM_KEY_F1: defaultDisplay(0); break;
  case SYSTEM_KEY_F2: defaultDisplay(1); break;
  case SYSTEM_KEY_F3: defaultDisplay(2); break;
  case SYSTEM_KEY_F4: defaultDisplay(3); break;

  case SYSTEM_KEY_F10:
    game->settings->camType = (game->settings->camType + 1) % CAM_COUNT;
    for(i = 0; i < game->players; i++)
      game->player[i].camera->camType = game->settings->camType;
    break;

  case SYSTEM_KEY_F5: saveSettings(); break;
  case SYSTEM_KEY_F12: doScreenShot(); break;

  default: 
    for(i = 0; i < KEY_ACTIONS_N; i++) {
      if(k == key_actions[i].key) {
	game->player[ key_actions[i].player ].data->turn = key_actions[i].turn;
	return;
      }
    }
    fprintf(stderr, "key %d is not bound\n", k);
  }
}

void parse_args(int argc, char *argv[]) {
  int i;
  while(argc--)
    if(argv[argc][0] == '-') {
      i = 0;
      while(argv[argc][++i] != 0) {
	switch(argv[argc][i]) {
	case 'k': game->settings->erase_crashed = 1; break;
	case 'f': game->settings->fast_finish = 1; break;
	case 'b': game->settings->show_alpha = 0; break;
	case 'm': game->settings->show_model = 0; break;
	case 'x': game->settings->show_crash_texture = 0; break;
	case 'F': game->settings->show_fps = 0; break;
	case 't': game->settings->show_floor_texture = 0; break;
	case 'c': game->settings->show_ai_status = 0; break;
	case 'g': game->settings->show_glow = 0; break;
	case 'w': game->settings->show_wall = 0; break;
	case 'C': game->settings->show_ai_status = 1; break;
	  /* case 'v': game->settings->screenSaver = 1; break; */
	case 'i': game->settings->windowMode = 1; break;
	case 'M': game->settings->mouse_warp = 1; break;
	case '1': /* default is -2 */
	  game->settings->width = 320;
	  game->settings->height = 240;
	  break;
	case '2': 
	  game->settings->width = 640;
	  game->settings->height = 480;
	  break;
	case '3': 
	  game->settings->width = 800;
	  game->settings->height = 600;
	  break;
	case '4': 
	  game->settings->width = 1024;
	  game->settings->height = 768;
	  break;
	case 's':
	  game->settings->playMusic = 0;
	  game->settings->playEffects = 0;
	  break;
	case 'h':
	default:
	  printf("Usage: %s [-FftwbghcCsk1234]\n\n", argv[0]);
	  printf("Options:\n\n");
	  printf("-k\terase crashed players (like in the movie)\n");
	  printf("-f\tfast finish after human has crashed\n");
	  printf("-F\tdon't display FPS counter\n");
	  printf("-t\tdon't display floor texture, use lines instead"
		 "(huge speed gain)\n");
	  printf("-w\tdon't display walls (speed gain)\n");
	  printf("-b\tdon't use blending (speed gain)\n");
	  printf("-m\tdon't show lightcycle (speed gain)\n");
	  printf("-x\tdon't show crash texture (speed gain)\n");
	  printf("-g\tdon't show glows (small speed gain)\n");
	  printf("-c\tdon't show ai status\n");
	  printf("-C\tshow ai status (default: on)\n");
	  printf("-1\tSet resolution to 320x240\n");
	  printf("-2\tSet resolution to 640x480 (default)\n");
	  printf("-3\tSet resolution to 800x600\n");
	  printf("-4\tSet resolution to 1024x768\n");
	  printf("-s\tDon't play sound\n");
	  /* printf("-v\tStart in demo/screensaver mode\n"); */
	  printf("-i\tforce startup in a window\n");
	  printf("-M\tcapture mouse (useful for Voodoo1/2 owners)\n");
	  printf("-h\tthis help\n");
	  exit(1);
	}
      }
    }
}
