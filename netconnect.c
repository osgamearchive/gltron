#include "gltron.h"


static int coffset;

void mouseConnect (int buttons, int state, int x, int y)
{
    if ( state == SYSTEM_MOUSEPRESSED )
      switchCallbacks(&guiCallbacks);
}

void keyConnect(int k, int x, int y)
{
  int status;

  switch( k )
    {
    case 13:
      status = Net_connect(game->settings->server, SERVER_PORT);
      isConnected = status;
      if( status != connected )
	{
	  restoreCallbacks();	  
	}
      Net_allocSocks();

      break;
    case 'k':
      Net_deconnect();
      restoreCallbacks();
      break;
    default:
      restoreCallbacks();
      //switchCallbacks(&guiCallbacks);
      break;
    }
}

void idleConnect() {
  SystemPostRedisplay();
  if( isConnected && Net_checkSocks() )
    {
      handleServer();
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
  Net_init();
}

void cleanConnect()
{
  Net_deconnect();
}

callbacks netConnectCallbacks = {
  displayConnect, idleConnect, keyConnect, initConnect,
  cleanConnect, NULL, mouseConnect, NULL
};
