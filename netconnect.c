#include "gltron.h"


static int coffset;

void mouseConnect (int buttons, int state, int x, int y)
{
    if ( state == SYSTEM_MOUSEPRESSED )
      switchCallbacks(&guiCallbacks);
}



void idleConnect() {
  int serverport = strtol(game->settings->port, (char**) NULL, 10);
  int status;
  SystemPostRedisplay();
  if( isConnected && Net_checksocks() )
    {
      handleServer();
    }
  
  if( ! isConnected )
    {
      status = Net_connect(game->settings->server, serverport);
      if( status )
	{
	  isConnected = 0;
	  restoreCallbacks();	  
	}
      Net_allocsocks();
      Net_addsocket(Net_getmainsock());
      login(game->settings->nickname);
    }
}
void keyConnect(int k, int unicode, int x, int y)
{
  if( k == SDLK_ESCAPE )
    {
      Net_disconnect();
      restoreCallbacks();
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
