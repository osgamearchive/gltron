#include "gltron.h"


static int coffset;

void mousePregame (int buttons, int state, int x, int y)
{
    if ( state == SYSTEM_MOUSEPRESSED )
      switchCallbacks(&guiCallbacks);
}

void keyPregame(int k, int unicode, int x, int y)
{
  if( k == 13 )
    {
      switchCallbacks(&keyboardreadingCallbacks);
    } else {
      switchCallbacks(&guiCallbacks);
    }
}

void idlePregame() {
  SystemPostRedisplay();
  if( isConnected && Net_checkSocks() )
    {
      handleServer();
    }
}


void drawPregame() {
  int time;
  int x, y;
  int h;
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 } };
  time = SDL_GetTicks() - coffset;
  
  glClearColor(.0, .0, .0, .0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);
  
  h = game->screen->vp_h / (24 * 1.5);
  glColor3fv(colors[1]);
  x = 10;
  y = game->screen->vp_h - 1.5 * h * (0 + 1);
  drawText(gameFtx, x, y, h, "Pregame: Type Enter to make command");
  
}

void displayPregame() {
  drawPregame();
  SystemSwapBuffers();
}

void initPregame() {
  coffset = SDL_GetTicks();
}

void cleanPregame()
{
  fprintf(stderr, "deconnecting...\n");
  Net_deconnect();
}

callbacks netPregameCallbacks = {
  displayPregame, idlePregame, keyPregame, initPregame,
  cleanPregame, NULL, mousePregame, NULL
};

