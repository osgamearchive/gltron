#include "gltron.h"


static int coffset;

void mouseScores (int buttons, int state, int x, int y)
{
  if ( state == SYSTEM_MOUSEPRESSED )
        switchCallbacks(&netPregameCallbacks);
}

void keyScores(int k, int unicode, int x, int y)
{
  printf("keyScores %c\n", k);
  switch( k )
    {
    case SYSTEM_KEY_F11: doBmpScreenShot(); break;
    case SYSTEM_KEY_F12: doScreenShot(); break;
    default: switchCallbacks(&netPregameCallbacks);break;
    }
}

void idleScores() {
  SystemPostRedisplay();
  if( isConnected && Net_checksocks() )
    {
      handleServer();
    }
}


void drawScores() {
  int time;
  int x, y;
  int h;
  int i, j;
  char str[255];
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 0.1, 0.1, 0.5} , { 0.0, 0.5, 1.0 }};
  time = SDL_GetTicks() - coffset;


  //Title
  h = game->screen->vp_h / (24 * 1.5);
  glColor3fv(colors[0]);
  x = 10;
  y = game->screen->vp_h - 1.5 * h ;
  drawText(gameFtx, x, y, h, "Scores ...");

  //Scores
  glColor3fv(colors[0]);
  x = 10;
  j=5;
  y = game->screen->vp_h - 1.5 * h * (j + 1);
  for(i=0; i<MAX_PLAYERS;++i)
    {
      if( slots[i].active == 1 )
	{
	  if( i == netscores.winner  )
	    glColor3fv(colors[1]);
	  
	  sprintf(str, "%c%s        :  %hd\n", i==netscores.winner?'*':'+',slots[i].name, netscores.points[i]);
	  drawText(gameFtx, x, y, h, str);
	  
	  y = game->screen->vp_h - 1.5 * h * (j+i + 2);
	  
	  if( i == netscores.winner )
	    glColor3fv(colors[0]);
	}  
    }
  
}

void displayScores() {
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawScores();
  SystemSwapBuffers();
}

void initScores() {
  coffset = SDL_GetTicks();
  printf("entering game scores, winner is %d\n", netscores.winner);
}

void cleanScores()
{
  //fprintf(stderr, "pregame: deconnecting...\n");
  //Net_deconnect();
}

void initGLScores() {
  glShadeModel(GL_FLAT);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  SystemPostRedisplay();
}


callbacks scoresCallbacks = {
  displayScores, idleScores, keyScores, initScores,
  cleanScores, initGLScores, mouseScores, NULL
};
