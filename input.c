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

void keyGame(int k, int unicode, int x, int y)
{
  int     i;
#ifdef __NETWORK__
  Packet  packet;
#endif

  switch (k) {
    /* case 'q': SystemExit(); break; */
  case 27:
#ifdef __NETWORK__
    if( game2->mode == GAME_NETWORK_PLAY  )
      {
	fprintf(stderr, "exit network game\n");
	isConnected=0;
	isLogged=0;
	Net_disconnect();
	serverstate=preGameState; //We hope that next time server will be to preGameState

	//TODO: see how not to came back to this callBack when doing lot of esc in gui!


    if( trackeruse == 1 )
      {
	if( ! tracker_connect() )
	  {
	    changeCallback(&trackerscreenCallbacks, &trackerscreenCallbacks);
	  } else {
	    changeCallback(&guiCallbacks, &guiCallbacks);
	  }
	//changeCallback(&guiCallbacks, &guiCallbacks);
	//changeCallback(&netPregameCallbacks, &netPregameCallbacks);
      } else {
#endif
	switchCallbacks(&pauseCallbacks);
	switchCallbacks(&guiCallbacks);
#ifdef __NETWORK__
      }
      }
#endif
    break;
  case ' ':
#ifdef __NETWORK__
    if( game2->mode == GAME_NETWORK_PLAY && ! isConnected )
#endif
	switchCallbacks(&pauseCallbacks);
    break;

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
#ifdef __NETWORK__
    if( unicode >= '1' && unicode <= '9' )
      {	//TODO: make chat possible to configure and change default
	//before game start...
	packet.which=me;
	packet.type=CHAT;
	packet.infos.chat.which=BROADCAST;
	switch( unicode )
	  {
	  case '1':
	    strcpy(packet.infos.chat.mesg, "You suck");
	    break;
	  case '2':
	    strcpy(packet.infos.chat.mesg, "Die!");
	    break;
	  case '3':
	    strcpy(packet.infos.chat.mesg, "blabla 3");
	    break;
	  case '4':
	    strcpy(packet.infos.chat.mesg, "blabla 4");
	    break;
	  case '5':
	    strcpy(packet.infos.chat.mesg, "blabla 5");
	    break;
	  case '6':
	    strcpy(packet.infos.chat.mesg, "blabla 6");
	    break;
	  case '7':
	    strcpy(packet.infos.chat.mesg, "blabla 7");
	    break;
	  case '8':
	    strcpy(packet.infos.chat.mesg, "blabla 8");
	    break;
	  case '9':
	    strcpy(packet.infos.chat.mesg, "blabla 9");
	    break;
	  }
	
	Net_sendpacket(&packet, Net_getmainsock());
	return;
      }
#endif
    for(i = 0; i < KEY_ACTIONS_N; i++) {
      if(k == key_actions[i].key) {
	int p;
	p = key_actions[i].player;
	if(game->player[p].data->speed > 0)
	  {
#ifdef __NETWORK__
	    if( game2->mode != GAME_NETWORK_PLAY  )
	      {
#endif
		createTurnEvent(p, key_actions[i].turn);
#ifdef __NETWORK__
	      } else {
		packet.which=me;
		packet.type=ACTION;
		printf("sending a turn ( current time is %d )\n", game2->time.current);
		if( key_actions[i].turn == TURN_LEFT )
		  packet.infos.action.type=TURNLEFT;
		else
		  packet.infos.action.type=TURNRIGHT;
		Net_sendpacket(&packet, Net_getmainsock());
		makeping(game2->time.current);
		doPredictedTurn(key_actions[i].turn, game2->time.current);
	      }
#endif
	  }
	return;
      }
    }

    sprintf(messages, "key '%s' is not bound", SystemGetKeyName(k));
    consoleAddLine(messages);
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
	case '5': 
	  game->settings->width = 1280;
	  game->settings->height = 1024;
	  break;
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
	  printf("-2\tSet resolution to 640x480 (default)\n");
	  printf("-3\tSet resolution to 800x600\n");
	  printf("-4\tSet resolution to 1024x768\n");
	  printf("-5\tSet resolution to 1280x1024\n");
	  printf("-s\tDon't play sound\n");
	  /* printf("-v\tStart in demo/screensaver mode\n"); */
	  printf("-i\tforce startup in a window\n");
	  printf("-O\toptimize for software rendering (SLOWER on "
		 "3D cards!)\n");
	  printf("-h\tthis help\n");
	  exit(1);
	}
      }
    }
  }
}
