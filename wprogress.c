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
  wprogress->last=0.0;

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
  //rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);


  glBegin(GL_LINES);
  glVertex2d(wprogress->x-1,wprogress->y-1);
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y-1);
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y-1);
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y+wprogress->height+1);
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wprogress->x+wprogress->width+1,wprogress->y+wprogress->height+1);	    
  glVertex2d(wprogress->x-1,wprogress->y+wprogress->height+1);	        
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wprogress->x-1, wprogress->y+wprogress->height+1 );
  glVertex2d(wprogress->x-1, wprogress->y-1);
  glEnd();

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
 
      glVertex2f(x, wprogress->y);
      glVertex2f(x, wprogress->y+wprogress->height);
      glEnd();
    }

  glLineWidth(1.0f);
}

void
update_wprogressbar(Wprogressbar *wprogress, float prog)
{
  int time=0;

  wprogress->last=wprogress->prog;
  wprogress->prog = prog;
  //time to wait
  time = (wprogress->prog-wprogress->last)*1000;
  printf("loading time %d progress %f-%f\n", time, wprogress->prog, wprogress->last);
  if( time > 100 )
    time = 100;
  if( time < 0 )
    time = 0;
  SDL_Delay(time);
}

void
free_wprogressbar(Wprogressbar *wprogress)
{
  if(wprogress ==NULL)
    return;

  free(wprogress);
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
 
  glColor4f(1.0, 1.0, 1.0, 1.0);
  drawText(gameFtx, wstatus->x, wstatus->y, 12, wstatus->status);
}

void
update_wprogressstatus(Wprogressstatus *wstatus, char *status)
{
  strcpy(wstatus->status, status);
}

void
free_wprogressstatus(Wprogressstatus *wstatus)
{
  if(wstatus ==NULL)
    return;

  free(wstatus->status);
  free(wstatus);
}
