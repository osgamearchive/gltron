#include "gltron.h"

Wbutton*  new_wbutton( int x, int y, int width, int height, 
		       char *title,
		       void ( *drawit ) (WbuttonRef wbutton),
		       void ( *action ) (WbuttonRef wbutton),
		       void ( *focus )  (WbuttonRef wbutton),
		       void (*mouseFocus) (WbuttonRef wbutton))
{
  Wbutton    *wbutton;


  if( title == NULL || strlen(title) > 254  )
      return NULL;

  wbutton = ( Wbutton * )malloc(sizeof(Wbutton));

  wbutton->x=x;
  wbutton->y=y;
  wbutton->width=width;
  wbutton->height=height;
  strcpy(wbutton->title, title);
  wbutton->drawit=drawit;
  wbutton->action=action;
  wbutton->focus=focus;
  wbutton->focused = 0;
  wbutton->mouseFocus=mouseFocus;

  return wbutton;
}



void
draw_wbutton(Wbutton *wbutton, int active)
{
  int x, y, s;
  if( wbutton->drawit != NULL )
    {
      wbutton->drawit(wbutton);
    } else {
      //draw a simple box
      if( wbutton->focused ) 
	{
	  if( wbutton->focus != NULL )
	    wbutton->focus(wbutton);
	  else
	    glColor3f(1.0, .2, .2);
	} else if( active )
	  glColor3f(1.0, 1.0, 1.0);
      else
	glColor3f(.5, .5, .5);

	//Draw Borders
	glBegin(GL_LINES);
	glVertex2d(wbutton->x,                  wbutton->y);
	glVertex2d(wbutton->x+wbutton->width,   wbutton->y);
	glEnd();


	glBegin(GL_LINES);
	glVertex2d(wbutton->x+wbutton->width,   wbutton->y);
	glVertex2d(wbutton->x+wbutton->width,   wbutton->y+wbutton->height);
	glEnd();


	glBegin(GL_LINES);
	glVertex2d(wbutton->x+wbutton->width,   wbutton->y+wbutton->height);
	glVertex2d(wbutton->x,                  wbutton->y+wbutton->height);
	glEnd();


	glBegin(GL_LINES);
	glVertex2d(wbutton->x,                  wbutton->y+wbutton->height);
	glVertex2d(wbutton->x,                  wbutton->y);
	glEnd();

	//and the title

	/* adjust size so menu fits into button width */

 /*  hsize = (int) ((float)d->vp_w * MENU_WIDTH / (float)maxw ); */
/*   vsize = (int) ((float)d->vp_h * MENU_HEIGHT /  */
/* 		 ( (float)pCurrent->nEntries * MENU_TEXT_LINEHEIGHT)); */

/*   size = (hsize < vsize) ? hsize : vsize; */
	//s = wbutton->width/(6.0 / 4.0 * strlen(wbutton->title));
	s = wbutton->width/(strlen(wbutton->title));
	x = wbutton->x + wbutton->width/2 - strlen(wbutton->title)*s/2;
	y = wbutton->y+wbutton->height/2-s/2;
	glColor3f(1.0, 1.0, 1.0);
	drawText(gameFtx, x, y, s, wbutton->title);
    }
    
}


void
free_wbutton(Wbutton *wbutton)
{
  free(wbutton);
}


Wrect
getRect_wbutton( Wbutton *wbutton )
{
  Wrect rect;

  rect.top    = wbutton->y + wbutton->height;
  rect.left   = wbutton->x;
  rect.bottom = wbutton->y;
  rect.right  = wbutton->x + wbutton->width;

  return rect;
}


void
mouse_wbutton( Wbutton *wbutton, int buttons, int state, int dblClick, Wpoint mousexy )
{
  if( buttons != 1 || state != SDL_PRESSED )
    return;

  if( wbutton->action != NULL )
    wbutton->action(wbutton);
}

void
mouseMotion_wbutton( Wbutton *wbutton, Wpoint mousexy, int active )
{
  wbutton->focused = active;
  //printf(" draw mouseMotion \n");
/*   if( wbutton->focus != NULL ) */
/*     { */
/*       wbutton->focus( wbutton ); */
/*     } else { */
/*       //draw a simple box */
/*       //printf("print the box\n"); */
/*       glColor3f(1.0, .2, .2); */
/*       glBegin(GL_LINES); */
/*       glVertex2d(wbutton->x-1,                  wbutton->y-1); */
/*       glVertex2d(wbutton->x+wbutton->width+1,   wbutton->y-1); */
/*       glEnd(); */
      
      
/*       glBegin(GL_LINES); */
/*       glVertex2d(wbutton->x+wbutton->width+1,   wbutton->y-1); */
/*       glVertex2d(wbutton->x+wbutton->width+1,   wbutton->y+wbutton->height+1); */
/*       glEnd(); */
      
      
/*       glBegin(GL_LINES); */
/*       glVertex2d(wbutton->x+wbutton->width+1,   wbutton->y+wbutton->height+1); */
/*       glVertex2d(wbutton->x-1,                  wbutton->y+wbutton->height+1); */
/*       glEnd(); */
      

/*       glBegin(GL_LINES); */
/*       glVertex2d(wbutton->x-1,                  wbutton->y+wbutton->height+1); */
/*       glVertex2d(wbutton->x-1,                  wbutton->y-1); */
/*       glEnd(); */
/*     } */
    
}
int
mouseFocus_wbutton( Wbutton *wbutton, Wpoint mousexy )
{
  //draw an help box
  if( wbutton->mouseFocus != NULL )
    {
      wbutton->mouseFocus(wbutton);
    }
  return 1;
}
