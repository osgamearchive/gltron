#include "gltron.h"


static int coffset;

void mouseConnect (int buttons, int state, int x, int y)
{
    if ( state == SYSTEM_MOUSEPRESSED )
	changeCallback(&guiCallbacks);
}



void idleConnect() {
  int serverport = strtol(game->settings->port, (char**) NULL, 10);
  int status;
  int sockstat = socksnotready;

  SystemPostRedisplay();

  if( isConnected==1  ) //game2->mode == GAME_NETWORK_PLAY && 
    {
      sockstat = Net_checksocks();
      //printf("sockstat = %d\n", sockstat);
      if( sockstat != socksnotready )
	{
	  if( sockstat & tcpsockready )
	    handleServer();
#ifdef USEUDP
	  if( sockstat & udpsockready )
	    printf("getting udp\n");
#endif
	}
    }

  
  if( ! isConnected )
    {
      status = Net_connect(game->settings->server, serverport);
      if( status )
	{
	  isConnected = 0;
	  fprintf(stderr, "can't connect to server\n");
	  restoreCallbacks();	  
	}
      Net_allocsocks();
      Net_addsocket(Net_getmainsock());
#ifdef USEUDP
      Net_addudpsocket(Net_getudpsock());
#endif
      printf(" isconnected = %d\n", isConnected);
      createTurnList();
      login(game->settings->nickname);
    }
}
void keyConnect(int k, int unicode, int x, int y)
{
  if( k == SDLK_ESCAPE )
    {
      Net_disconnect();
      changeCallback(&guiCallbacks);
    }
}

void drawConnect() {
  int time;
  int x, y;
  int h;
  char  connectMesg[255];
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 } };
  time = SDL_GetTicks() - coffset;

  glClearColor(.0, .0, .0, .0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);
  h = game->screen->vp_h / (24 * 1.5);
  glColor3fv(colors[1]);
  x = 10;
  y = game->screen->vp_h - 1.5 * h * (0 + 1);
  sprintf(connectMesg, "Connecting to %s ( %s )",game->settings->server, game->settings->nickname);
  drawText(gameFtx, x, y, h, connectMesg);
  if( isConnected )
    {
      y = game->screen->vp_h - 1.5 * h * (1 + 1);
      drawText(gameFtx, x, y, h, "Connected");
    }
  
}

void displayConnect() {
  drawConnect();
  SystemSwapBuffers();
}

void initConnect() {
  coffset = SDL_GetTicks();
}

void cleanConnect()
{
}

callbacks netConnectCallbacks = {
  displayConnect, idleConnect, keyConnect, initConnect,
  cleanConnect, NULL, mouseConnect, NULL
};
