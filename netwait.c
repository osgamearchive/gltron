#include "gltron.h"


static int coffset;


void mouseNetwait (int buttons, int state, int x, int y)
{
}

void keyNetwait(int k, int unicode, int x, int y)
{
  switch( k )
    {
    case SDLK_ESCAPE:
      fprintf(stderr, "exit network game\n");
      switchCallbacks(&guiCallbacks);      
      //TODO: see how not to came back to this callBack when doing lot of esc in gui!
      break;
      
    }
}

void idleNetwait() {
  SystemPostRedisplay();
}


void drawNetwait() {
  int time;
  int x, y;
  int h;
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 } };
  time = SDL_GetTicks() - coffset;
  
  glClearColor(.0, .0, .0, .0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);
  
  //Message
  h = game->screen->vp_h / (24 * 1.5);
  glColor3fv(colors[1]);
  x = 50;
  y = game->screen->vp_h - 1.5 * h * 12;
  drawText(gameFtx, x, y, h, "Game has already started");
  y = game->screen->vp_h - 1.5 * h * 13;
  drawText(gameFtx, x, y, h, "Try again later...\n");
}

void displayNetwait() {
  drawNetwait();
  SystemSwapBuffers();
}

void initNetwait() {
  coffset = SDL_GetTicks();

  printf("entering netwait\n");
}

void cleanNetwait()
{
}

callbacks netWaitCallbacks = {
  displayNetwait, idleNetwait, keyNetwait, initNetwait,
  cleanNetwait, NULL, mouseNetwait, NULL
};

