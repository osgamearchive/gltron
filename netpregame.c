#include "gltron.h"

static char *speed_list[] = {  "boring", "normal", "fast", "crazy", NULL };
static char *arena_list[] = { "tiny", "medium", "big", "vast", "extreme", NULL };

static int coffset;

static char message[255] ="pregame";
static char chat[1024]   = "";

#define MAX_CHARS 31

void
handlecommand(char *command, char *params)
{
  Packet   packet;
  char     *str;
  int      i;

  //check commands here.
  switch( command[0] )
    {
    case 's'://start
      if( serverstate == preGameState && isConnected )
	{
	  if( slots[me].isMaster )
	    {
	      printf("\nAsk to start the game\n");	      
	      makeping(game2->time.current);
	      printf("ping ask when start game is%d\n", getping());
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=STARTGAME;
	      Net_sendpacket(&packet, Net_getmainsock());
	    } else {
	      fprintf(stderr,"\nYour are not allowed to start the game, u must be Game Master\n");
	    }
	}
      break;
    case 'w':
      if( serverstate == preGameState && isConnected  )
	{
	  //Whisper
	  packet.which=me;
	  packet.type=CHAT;

	  //Find dest
	  str = strtok(params, " ");
	  printf("wisper to %s\n", str);
	  packet.infos.chat.which=BROADCAST;
	  for(i=0; i < MAX_PLAYERS; ++i)
	    {
	      if( ! strcasecmp(slots[i].name, str) )
		{
		  packet.infos.chat.which=i;
		}
	    }
	  if( packet.infos.chat.which==BROADCAST )
	    {
	      fprintf(stderr, "User %s doesn't exist.\n", str);
	      return;
	    }
	  str = strtok(NULL, " ");
	  strcpy(packet.infos.chat.mesg, str);
	  Net_sendpacket(&packet, Net_getmainsock());	  
	}
      break;
    case 'g': //Change Netsettings nbWins
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGENBWINS;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
    case 't': //Change Netsettings timeout
      if( serverstate == preGameState && isConnected  )
	{
	  
	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGETIMEOUT;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      if( packet.infos.action.which < 60 && packet.infos.action.which > 0 )
		{
		  Net_sendpacket(&packet, Net_getmainsock());
		} else {
	      fprintf(stderr,"\nValue is out of bound..\n");		  
		}	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
      case 'v': //Change Netset speed
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGESPEED;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
      case 'z': //Change Netsettings nbWins
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGESIZE;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
      case 'e': //Change Netsettings nbWins
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGEERASE;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
    case 'h': //print help
      insert_wtext(pregametext, "s : start a game\nw : wipser a player\ng : change nbwins settings\nt : change timeout ( minutes)\nv : change game speed ( 0 to 3 )\nz : change arena size ( 0 to 4 )\ne : change erased Crashed\n\n", 0);
      break;
    default:
      insert_wtext(pregametext, "unknown command\ntype /h for help\n", 0);
      break;
    }
}
void
keyboardreadingreturn(char *buff)
{
  char command[MAX_CHARS]="", params[MAX_CHARS]="";
  Packet packet;

  if( strlen(buff) == 0 )
    return;

  //We have our buffer.
  sscanf(buff, "/%[A-Za-z] %[A-Za-z0-9 ]", command, params);
  if( strlen(command) > 0 )
    {
      if( strlen(command) == 1 )
	handlecommand(command, params);
      else
	insert_wtext(pregametext, "unknown command\ntype /h for help\n", 0);

      //strcpy(buff, "");
      //fprintf(stderr, "\ncommand: %s\nparams: %s\n", command, params);
    } else {
      fprintf(stderr, "\nsend chat: %s\n", buff);
      packet.which=me;
      packet.type=CHAT;
      packet.infos.chat.which=BROADCAST;
      strcpy(packet.infos.chat.mesg, buff);
      Net_sendpacket(&packet, Net_getmainsock());
    }
  //strcpy(buff, "");
}



void mousePregame (int buttons, int state, int x, int y)
{
  //if ( state == SYSTEM_MOUSEPRESSED )
      //  switchCallbacks(&guiCallbacks);
}

void keyPregame(int k, int unicode, int x, int y)
{
  char *str;
  switch( k )
    {
    case SYSTEM_KEY_F11: doBmpScreenShot(); break;
    case SYSTEM_KEY_F12: doScreenShot(); break;
    case 13:
      str = get_wintext(inpregametext);
      keyboardreadingreturn(str);
      free(str);
      str=NULL;
      clear_wintext(inpregametext);
      break;
    case SDLK_ESCAPE:
      fprintf(stderr, "exit network game\n");
      isConnected=0;
      isLogged=0;
      Net_disconnect();
      serverstate=preGameState; //We hope that next time server will be to preGameState
      free_wtext(pregametext);
      pregametext=NULL;
      free_wintext(inpregametext);
      inpregametext=NULL;
      if( trackeruse == 1 )
	{
	  if( ! tracker_connect() )
	    changeCallback(&trackerscreenCallbacks, &trackerscreenCallbacks);
	  else
	    changeCallback(&guiCallbacks, &guiCallbacks);
	}
      else
	changeCallback(&guiCallbacks, &guiCallbacks);
      //TODO: see how not to came back to this callBack when doing lot of esc in gui!
      break;
    default:
      key_wintext(inpregametext, k, unicode);
      break;
    }
}

void idlePregame() {
  int sockstat;

  SystemPostRedisplay();

  sockstat = Net_checksocks();
  if(  isConnected && sockstat != socksnotready ) //game2->mode == GAME_NETWORK_PLAY &&
    {
      if( sockstat & tcpsockready )
	handleServer();
#ifdef USEUDP
      if( sockstat & udpsockready )
	handleUDP();
#endif
    }
}

void
drawMessage(char *str)
{
  strcpy(message, str);
}

void
drawChat(char *str)
{
  //TODO: multiline chat...
  strcpy(chat, str);
}

void drawPregame() {
  int time;
  int x, y;
  int h;
  int i, len;
  char str[255];
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0, 0.5, 0.5} , { 0.0, 0.5, 1.0 }};
  time = SDL_GetTicks() - coffset;
  
  //glClearColor(.0, .0, .0, .0);
  //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //rasonly(game->screen);
  
  h = game->screen->vp_h / (24 * 1.5);
  //Message
/*   glColor3fv(colors[1]); */
/*   x = 10; */
/*   y = game->screen->vp_h - 1.5 * h * (0 + 1); */
/*   drawText(gameFtx, x, y, h, message); */

  //chat
/*   glColor3fv(colors[0]); */
/*   x = 10; */
/*   y = game->screen->vp_h - 1.5 * h * (5 + 1); */
/*   drawText(gameFtx, x, y, h, chat); */
  
  //Draw pregametext
  draw_wtext(pregametext);
  
  //calculate the max len of a name;
  len=5; //for empty;
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if( slots[i].active && strlen( slots[i].name)>len)
	{
	  len= strlen( slots[i].name);
	}
    }
  glColor3fv(colors[1]);
  x = game->screen->vp_w - 1.5 * 16 *( game->screen->vp_w / (50 * 1.5) );
  y = game->screen->vp_h - 1.5 * h * (8);
  drawText(gameFtx, x, y, h, "Users");

  //Users
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      y = game->screen->vp_h - 1.5 * h * (i + 9);
      if( slots[i].active == 1 )
	{
	  if( slots[i].isMaster )
	    glColor3fv(colors[3]);
	    
	  sprintf(str, "%s (%d)\n", slots[i].name, slots[i].ping);
	  drawText(gameFtx, x, y, h, str);
	  if( slots[i].isMaster )
	    glColor3fv(colors[1]);
	  
	} else {
	  drawText(gameFtx, x, y, h, "Empty");
	}
    }

  //Inputs
  glColor3fv(colors[2]);
/*   x = 10; */
  //x = 20;
  /* x =  2 *( game->screen->vp_w / (50 * 1.5) ); */
/*   y = h; */
/*   drawText(gameFtx, x, y, h, getInputEntry()); */
  draw_wintext(inpregametext);

  //NetRules
  glColor3fv(colors[1]);
  x = game->screen->vp_w - 1.5 * 21*( game->screen->vp_w / (50 * 1.5) );
  y = game->screen->vp_h - 1.5 * h * 17;
  drawText(gameFtx, x, y, h, "Game Settings"); 
  sprintf(str, "Games: %d", netrulenbwins);
  y = game->screen->vp_h - 1.5 * h * 18; 
  drawText(gameFtx, x, y, h, str);
  sprintf(str, "Time: %d", netruletime);
  y = game->screen->vp_h - 1.5 * h * 19; 
  drawText(gameFtx, x, y, h, str);

  //GameRules 
  sprintf(str, "eraseCrashed: %d", game2->rules.eraseCrashed);
  y = game->screen->vp_h - 1.5 * h * 20; 
  drawText(gameFtx, x, y, h, str);
  sprintf(str, "Speed: %s", speed_list[game->settings->game_speed]);
  y = game->screen->vp_h - 1.5 * h * 21; 
  drawText(gameFtx, x, y, h, str);
  sprintf(str, "arena size: %s", arena_list[game->settings->arena_size]);
  y = game->screen->vp_h - 1.5 * h * 22; 
  drawText(gameFtx, x, y, h, str);

  
  
}

void drawPregameBackground() {
  checkGLError("pregame background start");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);

  if(game->settings->softwareRendering) {
    glRasterPos2i(0, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(game->screen->vp_w, game->screen->vp_h,
		 GL_RGB, GL_UNSIGNED_BYTE,
		 game->screen->pixelGui);
  } else {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_PREGAME]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);
    glVertex2f(0, 0);

    glTexCoord2f(1.0, 0.0);
    glVertex2f(game->screen->vp_w, 0);

    glTexCoord2f(1.0, .75);
    glVertex2f(game->screen->vp_w, game->screen->vp_h);

    glTexCoord2f(0.0, .75);
    glVertex2f(0, game->screen->vp_h);

    glEnd();
  }
}


void displayPregame() {
  drawPregameBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawPregame();
  SystemSwapBuffers();
}

void initPregame() {
  int top_left_x, top_left_y;
  int width, height;

  coffset = SDL_GetTicks();

  //Reinit scores
  netscores.winner = -1;

  if( pregametext == NULL )
    {
      //make position relative to screen size in percent
      top_left_x = 3*game->screen->vp_w /100 ;
      top_left_y = 75*game->screen->vp_h /100 ;

      //make size relative to screen size in percent.
      width     = 55*game->screen->vp_w /100 ;
      height    = 74*game->screen->vp_h /100;

      pregametext = new_wtext(width, height, top_left_x, top_left_y, 20);
      insert_wtext(pregametext, "connected...\n", 3);
      insert_wtext(pregametext, server_message, 7);
    }

  if( inpregametext == NULL )
    {
     //make position relative to screen size in percent
      top_left_x = 3*game->screen->vp_w /100 ;
      top_left_y = 2*game->screen->vp_h /100 ;

      //make size relative to screen size in percent.
      width     = 55*game->screen->vp_w /100 ;
      height    = 6*game->screen->vp_h /100;

      //Wintext *new_wintext(int width, int height, int posx, int posy, int nbchars, int maxchars);
      inpregametext = new_wintext(width, height, top_left_x, top_left_y, 20, MAX_CHARS);
    }
  resetScores();
  printf("entering netpregame\n");
}

void cleanPregame()
{
  //free_wtext(pregametext);
  //pregametext=NULL;
  //fprintf(stderr, "pregame: deconnecting...\n");
  //Net_deconnect();
}

callbacks netPregameCallbacks = {
  displayPregame, idlePregame, keyPregame, initPregame,
  cleanPregame, NULL, mousePregame, NULL
};

