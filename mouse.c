#include "gltron.h"

static int x=0;
static int y=0;

/* x=game->screen->vp_w/2; */
/* y=game->screen->vp_h/2; */

void drawMouse(  )
{
  checkGLError("cursor start");

/*   //first draw a cross ( for testing purpose ) */
/*   glColor3f(1.0, .4, .4); */

/*    //Draw Borders */
/*   glBegin(GL_LINES); */
/*   glVertex2d(x-2,   y); */
/*   glVertex2d(x+3,   y); */
/*   glEnd(); */

/*  //Draw Borders */
/*   glBegin(GL_LINES); */
/*   glVertex2d(x,   y-2); */
/*   glVertex2d(x,   y+3); */
/*   glEnd(); */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_MOUSECURSOR]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //printf("error:%s\n", checkGLError());

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(x, y-32);

    glTexCoord2f(0, 1);
    glVertex2f(x, y);

    glTexCoord2f(1, 1);
    glVertex2f(x+32, y);

    glTexCoord2f(1, 0);
    glVertex2f(x+32, y-32);

    glEnd();
    glDisable(GL_BLEND);
}

void
setMouse( int nx, int ny )
{
  x=nx;
  y=game->screen->vp_h-ny;
  if( y < 0 )
    y = 0;
  if( x < 0 )
    x = 0;
  if( y > game->screen->vp_h )
    y = game->screen->vp_h;
  if( x > game->screen->vp_w )
    x = game->screen->vp_w;
}


void getMouse( int *nx, int *ny )
{
  *nx = x;
  *ny = y;
}

