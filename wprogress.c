#include "gltron.h"
#include <GL/glu.h>

Wprogressbar*
new_wprogressbar(int x, int y, int width, int height)
{
  Wprogressbar  *wprogress;

  //allocate memory
  wprogress = (  Wprogressbar  *) malloc(sizeof(Wprogressbar));
  if( wprogress == NULL )
    {
      fprintf(stderr, "could not allocate memory for wprogress\n");
      return NULL;
    }

  //init
  wprogress->x=x;
  wprogress->y=y;
  wprogress->width=width;
  wprogress->height=height;
  wprogress->prog=0.0;

  return wprogress;
}

void
draw_wprogressbar(Wprogressbar *wprogress)
{
  int i;
  float s = 0.0;
  float x = 0.0;
  float colors[][4] = { { 0.6, 0.0, 0.0, 0.6 }, { 0.1, 0.1, 0.6, 0.6 }, { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 0.0 , 0.0, 0.0} };


  //Drawing box
  //finding color
  rasonly(game->screen);	  
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glColor3f(1.0, 1.0, 1.0);


  glBegin(GL_LINES);
  glVertex2d(wprogress->x-1,wprogress->y-1);	                // Left Side Of Horizontal Line
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y-1);	// Right Side Of Horizontal Line
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y-1);	                // Left Side Of Horizontal Line
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y+wprogress->height+1);	// Right Side Of Horizontal Line
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y+wprogress->height+1);	    
  glVertex2d(wprogress->x-1,wprogress->y+wprogress->height+1);	        
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wprogress->x-1, wprogress->y+wprogress->height+1 );
  glVertex2d(wprogress->x-1, wprogress->y-1);
  glEnd();




  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
/*   glDepthMask(GL_FALSE); */
/*   glDisable(GL_DEPTH_TEST);	   */
  
/*   glEnable(GL_BLEND); */
/*   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */


/*   //setColor4fv(game->player[i); */
/*   glShadeModel(GL_SMOOTH); */
  
/*   setColor4fv(colors[0]); */
/*   light4fv(color); */
/*   glColor4fv(color); */

  s=(float)wprogress->width/100.0;
  //printf("s=%f\n", s);
  glLineWidth(s);

  for(i=0;i<100; ++i)
    {
      glBegin(GL_LINES);
      if( i < wprogress->prog *100 )
	{
	  glColor4fv(colors[0]);
	} else {
	  glColor4fv(colors[1]);
	}
      x = (float)wprogress->x+i*s+1;
      
      // printf("x=%f\n", x);
      glVertex2f(x, wprogress->y);	                        // Left Side Of Horizontal Line
      glVertex2f(x, wprogress->y+wprogress->height);	// Right Side Of Horizontal Line
      //fprintf(stderr, "drawing progress x: %f to %d\n", wprogress->x+i*s, wprogress->width/2+game->screen->vp_w/2);
      //fprintf(stderr, "drawing progress y: %d to %d\n", wprogress->width/2 - game->screen->vp_w/2, wprogress->y+wprogress->height);
      glEnd();
    }

  glDisable(GL_BLEND);
  
  //glEnable(GL_DEPTH_TEST);
  //glDepthMask(GL_TRUE);

  glLineWidth(1.0f);
}

void
update_wprogressbar(Wprogressbar *wprogress, float prog)
{
  int time;

  wprogress->prog = prog;
  printf("loading time %d\n", time);
  //time to wait
  time = (wprogress->prog-wprogress->last)*1000;
  if( time > 100 )
    time = 100;
  SDL_Delay(time);
}


Wprogressstatus*
new_wprogressstatus(int x, int y, int nbchars)
{
  Wprogressstatus *wstatus;

  //allocate memory
  wstatus = ( Wprogressstatus *) malloc(sizeof(Wprogressstatus));
  if( wstatus == NULL )
    {
      fprintf(stderr, "could not allocate memory for wprogress\n");
      return NULL;
    }

  //init
  wstatus->x       = x;
  wstatus->y       = y;
  wstatus->nbchars = nbchars;
  wstatus->status  = (char *)malloc(nbchars+1);
  strcpy(wstatus->status, "loading arts");
 
  return wstatus;
}


void
draw_wprogressstatus(Wprogressstatus  *wstatus)
{
  glColor3f(1.0, 1.0, 1.0);
  drawText(gameFtx, wstatus->x, wstatus->y, 12, wstatus->status);
}
void

update_wprogressstatus(Wprogressstatus *wstatus, char *status)
{
  strcpy(wstatus->status, status);
}
