#include "gltron.h"

static int x;
static int y;

/* x=game->screen->vp_w/2; */
/* y=game->screen->vp_h/2; */

void drawMouse(  )
{
  checkGLError("cursor start");

  //first draw a cross ( for testing purpose )
  glColor3f(1.0, .4, .4);

   //Draw Borders
  glBegin(GL_LINES);
  glVertex2d(x-2,   y);
  glVertex2d(x+3,   y);
  glEnd();

 //Draw Borders
  glBegin(GL_LINES);
  glVertex2d(x,   y-2);
  glVertex2d(x,   y+3);
  glEnd();
}

void
setMouse( int nx, int ny )
{
  x=nx;
  y=game->screen->vp_h-ny;
}


