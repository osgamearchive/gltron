#include "gltron.h"

Wbutton*  new_wbutton( int x, int y, int width, int height, 
		       char *title,
		       void ( *drawit ) (WbuttonRef wbutton),
		       void ( *action ) (WbuttonRef wbutton),
		       void ( *focus )  (WbuttonRef wbutton))
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

  return wbutton;
}



void
draw_wbutton(Wbutton *wbutton)
{
  if( wbutton->drawit != NULL )
    {
      wbutton->drawit(wbutton);
    } else {
      //draw a simple box
        glColor3f(1.0, 1.0, 1.0);

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
	drawText(gameFtx, wbutton->x+1, wbutton->y+1, 10, wbutton->title);
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

  rect.top    = wbutton->y;
  rect.left   = wbutton->x;
  rect.bottom = wbutton->y + wbutton->height;
  rect.right  = wbutton->x + wbutton->width;

  return rect;
}
