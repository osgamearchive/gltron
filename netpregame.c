#include "gltron.h"

static char *speed_list[] = {  "boring", "normal", "fast", "crazy", NULL };
static char *arena_list[] = { "tiny", "medium", "big", "vast", "extreme", NULL };
static char *ai_list[] = { "dumb", "normal", "strong", "the MCP", NULL };
static int oldType;

#define MAX_CHARS 255

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
	  if( strlen(str) > 31 )
	    {
	      str[31]='\0';
	    }
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
    case 'a': //Ask server to add AI player
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      printf("asking for ai player\n");
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=ADDAIPLAYER;
	      str = strtok(params, " ");
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
    case 'l':
      if( serverstate == preGameState && isConnected  )
	{
	  if( slots[me].isMaster )
	    {
	      printf("changing ai level\n");
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=AILEVEL;
	      str = strtok(params, " ");
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
    case 'h': //print help
      insert_wtext(pregame.pregametext, "s : start a game\nw : wipser a player\ng : change nbwins settings\nt : change timeout ( minutes)\nv : change game speed ( 0 to 3 )\nz : change arena size ( 0 to 4 )\ne : change erased Crashed\na : add ai player ( nb ai player )\nl : change ai level ( 0 to 3)\n\n", 0);
      break;
    default:
      insert_wtext(pregame.pregametext, "unknown command\ntype /h for help\n", 0);
      break;
    }
}
void
keyboardreadingreturn(char *buff)
{
  char command[MAX_CHARS]="", params[MAX_CHARS]="";
  Packet packet;
  int    i, j;

  if( strlen(buff) == 0 )
    return;

  //We have our buffer.
  sscanf(buff, "/%[A-Za-z] %[A-Za-z0-9 ]", command, params);
  if( strlen(command) > 0 )
    {
      if( strlen(command) == 1 )
	handlecommand(command, params);
      else
	insert_wtext(pregame.pregametext, "unknown command\ntype /h for help\n", 0);

      //strcpy(buff, "");
      //fprintf(stderr, "\ncommand: %s\nparams: %s\n", command, params);
    } else {
      fprintf(stderr, "\nsend chat: %s\n", buff);
      packet.which=me;
      packet.type=CHAT;
      packet.infos.chat.which=BROADCAST;
      j=strlen(buff);
      i=0;
      do {
	  if( j > 31 )
	    {
	      strncpy(packet.infos.chat.mesg, &buff[31*i], 31);	    
	      packet.infos.chat.mesg[31]='\0';
	    } else {
	      strncpy(packet.infos.chat.mesg, &buff[31*i], j);	    
	      packet.infos.chat.mesg[j]='\0';
	    }
	  Net_sendpacket(&packet, Net_getmainsock());
	  j-=31;
	  ++i;
       } while (  j > 0 );
     
    }
}



void mousePregame (int buttons, int state, int x, int y)
{
  Wpoint pt;
  pt.v=y;
  pt.h=x;
  clickControls(pregame.pregameControls, buttons, state, pt);
}

void keyPregame(int k, int unicode, int x, int y)
{
  char *str;
  switch( k )
    {
    case SYSTEM_KEY_F11: doBmpScreenShot(); break;
    case SYSTEM_KEY_F12: doScreenShot(); break;
    case 13:
      str = get_wintext(pregame.inpregametext);
      keyboardreadingreturn(str);
      free(str);
      str=NULL;
      clear_wintext(pregame.inpregametext);
      break;
    case SDLK_ESCAPE:
      fprintf(stderr, "exit network game\n");
      isConnected=0;
      isLogged=0;
      Net_disconnect();
      serverstate=preGameState; //We hope that next time server will be to preGameState
      freeRootControl(pregame.pregameControls);
      pregame.pregameControls=NULL;
      pregame.userslist=NULL;
      pregame.gameType=NULL;
      pregame.pregametext =NULL;
      pregame.inpregametext=NULL;
      pregame.gameRule=NULL;
      pregame.erase=NULL;
      pregame.speed=NULL;
      pregame.size=NULL;
      pregame.level=NULL;
      pregame.start=NULL;
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
      keyControls(pregame.pregameControls, k, unicode);
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
drawUsersListBox(WlistPtr list, int x, int y, int line, int col)
{
  int     h = list->height/list->nblines;
  float   colors[][3] = { {.4, .1, .1}, { .7, .1, .1 }, { .7, .7, .1 }, { .1, .7, .1 } };

  if( line < 0 || line > MAX_PLAYERS || slots[line].active != 1 )
    return;

  if( slots[line].isMaster )
    {
      //Draw a red background
      glColor3fv(colors[0]);
      glBegin(GL_QUADS);	  
      glVertex3f(x+1, y+h/2+1, 0.0f);     //top left
      if( col == list->nbcols-1 )
	{
	  glVertex3f(x+ list->width*list->colDefs[col].colsize/100-1, y+h/2+1, 0.0f);   //top right
	  glVertex3f(x+ list->width*list->colDefs[col].colsize/100-1, y-h/2-1, 0.0f);   //Bottom right
	} else {
	  glVertex3f(x+ list->width*list->colDefs[col].colsize/100+1, y+h/2+1, 0.0f);   //top right
	  glVertex3f(x+ list->width*list->colDefs[col].colsize/100+1, y-h/2-1, 0.0f);   //Bottom right
	}
      glVertex3f(x+1, y-h/2-1, 0.0f);         //Bottom left
      glEnd();
    }

  //draw ping
  glLineWidth(4.0f);

  if( slots[line].ping <= 0 )
    {      
      checkGLError("cross ping == 0 1");
      glColor3fv(colors[1]);
      //Draw a cross
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5,    y+h/2-8 );
      glVertex2d(x+h*3.5+8,  y-h/2+8 );
      glEnd();
      checkGLError("cross ping == 0 2");
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5,   y-h/2+8   );
      glVertex2d(x+h*3.5+8, y+h/2-8  );
      glEnd();
      glLineWidth(1.0f);
      return;
    }

  
  glColor3fv(colors[3]);
  if( slots[line].ping > 0 )
    {
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5,    y+h/2-8 );
      glVertex2d(x+h*3.5,    y-h/2+8 );
      glEnd();
    }
  if( slots[line].ping > 50 )
    {
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5+8,    y+h/2-8 );
      glVertex2d(x+h*3.5+8,    y-h/2+8 );
      glEnd();
    }
  if( slots[line].ping > 100 )
    {
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5+8*2,    y+h/2-8 );
      glVertex2d(x+h*3.5+8*2,    y-h/2+8 );
      glEnd();
    }
  glColor3fv(colors[2]);
  if( slots[line].ping > 150 )
    {
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5+8*3,    y+h/2-8 );
      glVertex2d(x+h*3.5+8*3,    y-h/2+8 );
      glEnd();
    }
  if( slots[line].ping > 200 )
    {
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5+8*4,    y+h/2-8 );
      glVertex2d(x+h*3.5+8*4,    y-h/2+8 );
      glEnd();
    }
  glColor3fv(colors[1]);
  if( slots[line].ping > 250 )
    {
      glBegin(GL_LINES);
      glVertex2d(x+h*3.5+8*5,    y+h/2-8 );
      glVertex2d(x+h*3.5+8*5,    y-h/2+8 );
      glEnd();
    }
  glLineWidth(1.0f);
}

void
updateUsersListData()
{
  int i;

  if(pregame.userslist == NULL )
    return;

  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if( slots[i].active == 1 )
	{
	  setCell_wlist( pregame.userslist, slots[i].name, strlen(slots[i].name)+1, i, 0);
	} else {
	  if( slots[i].active == 2 )
	    setCell_wlist( pregame.userslist, "Computer", 9, i, 0);
	  else
	    setCell_wlist( pregame.userslist, "Empty", 6, i, 0);
	}
    }
}

void drawPregameBackground() {
  checkGLError("pregame background start");

  

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

  rasonly(game->screen);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  drawPregameBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();

  updateControls(pregame.pregameControls);
  drawMouse();
  SystemSwapBuffers();
}


void
startaction( Wbutton *wbutton )
{
  Packet packet;

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
}

void
startMouseFocus( Wbutton *wbutton )
{  
  float color[4] = { 0.1f, 0.2f, 0.8f, .8f };
  int   x, y;
  int   width    = 210;
  //int   h      = wbutton->height;
  int   s        = 8;
  int   height   = s*3;
  char  str[255];

  //draw a beautifull transparent box using alpha trucs
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv(color);

  //find pos x, y
  x = wbutton->x + wbutton->width/3;
  y = wbutton->y + 10;
  glBegin(GL_QUADS); 
  glVertex3f(x, y, 0.0f);   //top left
  glVertex3f(x+width, y, 0.0f);   //top right
  glVertex3f(x+width, y-height, 0.0f);  //Bottom right
  glVertex3f(x, y-height, 0.0f);  //Bottom left  
  glEnd();
  glDisable(GL_BLEND);

  glColor3f(1.0,1.0, 1.0);
  glBegin(GL_LINES);
  glVertex2d(x,y);
  glVertex2d(x+width, y);
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(x+width, y);
  glVertex2d(x+width, y-height);
  glEnd();
  
      
  glBegin(GL_LINES);
  glVertex2d(x+width, y-height);
  glVertex2d(x, y-height);
  glEnd();
  

  glBegin(GL_LINES);
  glVertex2d(x, y-height);
  glVertex2d(x, y);
  glEnd();

  y -=2*s;
  x+=10;
  sprintf(str, "Starting gltron's Server");
  drawText(gameFtx, x, y, s, str);
  y -=s;
}


void changeGameType( Wpopmenu *wpopmenu )
{
  int    type;
  Packet packet;

  //Here we change game type.
  type = getValue_wpopmenu(wpopmenu);
  printf("oldType = %d, new type = %d\n", oldType, type);

  switch( type )
    {
    case 0:
      if( type == oldType )
	return;
      setTitle_wpopmenu(pregame.gameRule, "Games Number");
      select_wpopmenu(pregame.gameRule, netrulenbwins/5-1 );
      netruletime=0;
      netrulenbwins=5;
	break;
    case 1:
      if( type == oldType )
	return;
      setTitle_wpopmenu(pregame.gameRule, "Time ( mins )");
      select_wpopmenu(pregame.gameRule, netruletime/5-1 );
      netrulenbwins=5;
      netruletime=5;
      break;
    default:
      fprintf(stderr, "an error occured\n");
      return;
    }
  oldType=type;

  //Send packets
  if( serverstate == preGameState && isConnected  )
    {
      if( slots[me].isMaster )
	{
	  packet.which=me;
	  packet.type=ACTION;
	  packet.infos.action.type=CHGENBWINS;
	  packet.infos.action.which = netrulenbwins;
	  Net_sendpacket(&packet, Net_getmainsock());
	  
	  packet.which=me;
	  packet.type=ACTION;
	  packet.infos.action.type=CHGETIMEOUT;
	  packet.infos.action.which = netruletime;
	  if( packet.infos.action.which < 60 && packet.infos.action.which >= 0 )
	    {
	      Net_sendpacket(&packet, Net_getmainsock());
	    } else {
	      fprintf(stderr,"\nValue is out of bound..%d\n", packet.infos.action.which);		  
	    }	  
	} else {
	  fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	}
    }
}


void changeGameRule( Wpopmenu *wpopmenu )
{
  int    type;
  int    value;
  Packet packet;

  //Here we change game type.
  type  = getValue_wpopmenu(pregame.gameType);
  value = getValue_wpopmenu(wpopmenu);

  switch( type )
    {
    case 0:
      packet.which=me;
      packet.type=ACTION;
      packet.infos.action.type=CHGENBWINS;
      packet.infos.action.which = (value+1)*5;
	break;
    case 1:
      packet.which=me;
      packet.type=ACTION;
      packet.infos.action.type=CHGETIMEOUT;
      packet.infos.action.which = (value+1)*5;
      break;
    default:
      fprintf(stderr, "an error occured\n");
      return;
    }
  Net_sendpacket(&packet, Net_getmainsock());
}

void changeSpeed( Wpopmenu *wpopmenu )
{
  int    value;
  Packet packet;
  value = getValue_wpopmenu(wpopmenu);

  packet.which=me;
  packet.type=ACTION;
  packet.infos.action.type=CHGESPEED;
  packet.infos.action.which = value;
  Net_sendpacket(&packet, Net_getmainsock());
}

void changeSize( Wpopmenu *wpopmenu )
{
  int    value;
  Packet packet;
  value = getValue_wpopmenu(wpopmenu);

  packet.which=me;
  packet.type=ACTION;
  packet.infos.action.type=CHGESIZE;
  packet.infos.action.which = value;
  Net_sendpacket(&packet, Net_getmainsock());
}

void changeLevel( Wpopmenu *wpopmenu )
{
  int    value;
  Packet packet;
  value = getValue_wpopmenu(wpopmenu);

  packet.which=me;
  packet.type=ACTION;
  packet.infos.action.type=AILEVEL;
  packet.infos.action.which = value;
  Net_sendpacket(&packet, Net_getmainsock());

}

void eraseAction( Wcheckbox *wcheckbox )
{
  Packet packet;

  packet.which=me;
  packet.type=ACTION;
  packet.infos.action.type=CHGEERASE;
  packet.infos.action.which = get_wcheckbox(wcheckbox );
  Net_sendpacket(&packet, Net_getmainsock());
}


void initPregame() {
  int top_left_x, top_left_y;
  int width, height;
  int x, y, h;
  int i;
  char empty[] = "Empty";
  char serveradd[1024];
  int port;
  char server[1024];
  ColDef    *colDefs;
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0, 0.5, 0.5} , { .4, 0.9, .4 }};


  //init OPENGL
  glShadeModel ( GL_FLAT       );
  glDisable    ( GL_BLEND      );
  glDisable    ( GL_LIGHTING   );
  glDisable    ( GL_DEPTH_TEST );


/* static Wlist        *userslist = NULL; */
/* static Wstatictext  *servertext = NULL; */

  if(pregame.pregameControls != NULL )
    return;

  //Root controls
  pregame.pregameControls = newRootControl();

  h = game->screen->vp_h / (24 * 1.5);

  //Creating the user list
  colDefs = new_colDefs( 1 );

  set_colDef( colDefs, 0, "Users", 100, colors[1], drawUsersListBox, charToStr_wlist, NULL);

  //x = game->screen->vp_w - 1.5 * 16 *( game->screen->vp_w / (50 * 1.5) );
  //y = game->screen->vp_h - 250;
  x = 67.7*game->screen->vp_w /100;
  y = 47*game->screen->vp_h /100;
  width=30*game->screen->vp_h /100;
  height=30*game->screen->vp_h /100;
  pregame.userslist = new_wlist(x, y,  width, height, 5, 1, colDefs, 0, NULL, NULL, NULL );

  //Default value
  i = addRow_wlist     ( pregame.userslist, 4 );
  if( i >= 0 )
    {
      setCell_wlist( pregame.userslist, empty, strlen(empty)+1, i, 0);
      setCell_wlist( pregame.userslist, empty, strlen(empty)+1, i+1, 0);
      setCell_wlist( pregame.userslist, empty, strlen(empty)+1, i+2, 0);
      setCell_wlist( pregame.userslist, empty, strlen(empty)+1, i+3, 0);
    }			
  newControl(pregame.pregameControls, (Wptr)pregame.userslist, Wlistbox);

  //Reinit scores
  netscores.winner = -1;

  //make position relative to screen size in percent
  top_left_x = 3*game->screen->vp_w /100 ;
  top_left_y = 75*game->screen->vp_h /100 ;
  
  //make size relative to screen size in percent.
  width     = 55*game->screen->vp_w /100 ;
  height    = 74*game->screen->vp_h /100;
  
  pregame.pregametext = new_wtext(width, height, top_left_x, top_left_y, 20);
  insert_wtext(pregame.pregametext, "connected...\n", 3);
  insert_wtext(pregame.pregametext, server_message, 7);
  newControl(pregame.pregameControls, (Wptr)pregame.pregametext, WoutputText);

  //make position relative to screen size in percent
  top_left_x = 3*game->screen->vp_w /100 ;
  top_left_y = 2*game->screen->vp_h /100 ;
  
  //make size relative to screen size in percent.
  width     = 55*game->screen->vp_w /100 ;
  height    = 6*game->screen->vp_h /100;

  pregame.inpregametext = new_wintext(width, height, top_left_x, top_left_y, 20, MAX_CHARS);
  newControl(pregame.pregameControls, (Wptr)pregame.inpregametext, WinputText);

  getServer(serveradd, &port);
  sprintf(server, "Server %s:%i", serveradd, port);
  printf("server is %s\n", server);
  top_left_x = game->screen->vp_w /2 - (strlen(server)/2)* (h+5);
  top_left_y = game->screen->vp_h - 7*h ;

  newControl(pregame.pregameControls, (Wptr)new_wstatictext( top_left_x, top_left_y, 1.5 * strlen(server) *( game->screen->vp_w / (50 * 1.5)), h+5, server, h+3, gameFont, colors[0]), WstaticText);
  
  //Settings now

  //game type
  pregame.gameType = new_wpopmenu(60*game->screen->vp_w /100, 35*game->screen->vp_h /100, 35*game->screen->vp_w /100, h+5, "Game Type   ", changeGameType);
  addchoice_wpopmenu(pregame.gameType, "Games", 0 );
  addchoice_wpopmenu(pregame.gameType, "Time",  1 );
  select_wpopmenu(pregame.gameType, 0 );
  newControl(pregame.pregameControls, (Wptr)pregame.gameType, WpopupMenu);
  oldType=0;


  //Game Rules
  pregame.gameRule= new_wpopmenu(60*game->screen->vp_w /100, 30*game->screen->vp_h /100, 35*game->screen->vp_w /100, h+5, "Games Number", changeGameRule);
  addchoice_wpopmenu(pregame.gameRule, "5",   0 );
  addchoice_wpopmenu(pregame.gameRule, "10",  1 );
  addchoice_wpopmenu(pregame.gameRule, "15",  2 );
  addchoice_wpopmenu(pregame.gameRule, "20",  3 );
  addchoice_wpopmenu(pregame.gameRule, "25",  4 );
  select_wpopmenu(pregame.gameRule, 0 );
  newControl(pregame.pregameControls, (Wptr)pregame.gameRule, WpopupMenu);


  pregame.erase= new_wcheckbox(60*game->screen->vp_w /100, 25*game->screen->vp_h /100, 35*game->screen->vp_w/100, h+5, "Erase Deads ", eraseAction);
  newControl(pregame.pregameControls, (Wptr)pregame.erase, WcheckBox);

  //Speed
  pregame.speed= new_wpopmenu(60*game->screen->vp_w /100, 20*game->screen->vp_h /100, 35*game->screen->vp_w/100, h+5, "Speed       ", changeSpeed);
  addchoice_wpopmenu(pregame.speed, speed_list[0],  0 );
  addchoice_wpopmenu(pregame.speed, speed_list[1],  1 );
  addchoice_wpopmenu(pregame.speed, speed_list[2],  2 );
  addchoice_wpopmenu(pregame.speed, speed_list[3],  3 );
  select_wpopmenu(pregame.speed, 0 );
  newControl(pregame.pregameControls, (Wptr)pregame.speed, WpopupMenu);

  //Size
  pregame.size= new_wpopmenu(60*game->screen->vp_w /100, 15*game->screen->vp_h /100, 35*game->screen->vp_w/100, h+5, "Size        ", changeSize);
  addchoice_wpopmenu(pregame.size, arena_list[0],  0 );
  addchoice_wpopmenu(pregame.size, arena_list[1],  1 );
  addchoice_wpopmenu(pregame.size, arena_list[2],  2 );
  addchoice_wpopmenu(pregame.size, arena_list[3],  3 );
  addchoice_wpopmenu(pregame.size, arena_list[4],  4 );
  select_wpopmenu(pregame.size, 0 );
  newControl(pregame.pregameControls, (Wptr)pregame.size, WpopupMenu);

  //Level
  pregame.level= new_wpopmenu(60*game->screen->vp_w /100, 10*game->screen->vp_h /100, 35*game->screen->vp_w/100, h+5, "AI level    ", changeLevel);
  addchoice_wpopmenu(pregame.level, ai_list[0],  0 );
  addchoice_wpopmenu(pregame.level, ai_list[1],  1 );
  addchoice_wpopmenu(pregame.level, ai_list[2],  2 );
  addchoice_wpopmenu(pregame.level, ai_list[3],  3 );
  select_wpopmenu(pregame.level, 0 );
  newControl(pregame.pregameControls, (Wptr)pregame.level, WpopupMenu);

  //Deactivation of controls. if we are the game master it'll be reactivate
  deActivateControl( pregame.pregameControls, (Wptr)pregame.level );
  deActivateControl( pregame.pregameControls, (Wptr)pregame.speed );
  deActivateControl( pregame.pregameControls, (Wptr)pregame.size  );
  deActivateControl( pregame.pregameControls, (Wptr)pregame.gameRule );
  deActivateControl( pregame.pregameControls, (Wptr)pregame.gameType );

  //The start button
  pregame.start = new_wbutton(70*game->screen->vp_w /100, 5*game->screen->vp_h /100, 100, 15, "Start game", NULL, startaction, NULL, startMouseFocus);
  newControl(pregame.pregameControls, (Wptr)pregame.start, WcontrolButton);
  deActivateControl( pregame.pregameControls, (Wptr)pregame.start );

  setCurrentControl(pregame.pregameControls, (Wptr)pregame.inpregametext);

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

static void
mousemotionPregame( int mx, int my )
{
  Wpoint pt;

  pt.h=mx;
  pt.v=my;

  setMouse( mx, my );
  mouseControls( pregame.pregameControls, pt );
}

callbacks netPregameCallbacks = {
  displayPregame, idlePregame, keyPregame, initPregame,
  cleanPregame, NULL, mousePregame, mousemotionPregame
};

