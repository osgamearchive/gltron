#include "gltron.h"

Wcheckbox*
new_wcheckbox( int x, int y, int width, int height, char *title,
			   void (*action)( Wcheckbox *wcheckbox ) )
{
  Wcheckbox  *wcheckbox;

  if( title == NULL )
    return NULL;

  wcheckbox = ( Wcheckbox  * ) malloc( sizeof(Wcheckbox) );

  wcheckbox->x = x;
  wcheckbox->y = y;
  wcheckbox->width = width;
  wcheckbox->height = height;
  strncpy(wcheckbox->title, title, 254);
  wcheckbox->checked = 0;
  wcheckbox->action = action;

  return wcheckbox;
}


void
draw_wcheckbox( Wcheckbox *wcheckbox, int active )
{
  int x, y;
  float color[4] = { 0.05f, 0.05f, 0.05f, .8f };

  //Draw title
  if( ! active )
    glColor3f(.5, .5, .5);
  else
    glColor3f(1.0, 1.0, 1.0);

  x=wcheckbox->x+10;
  y=wcheckbox->y+4;
  drawText(gameFtx, x, y, wcheckbox->height-8, wcheckbox->title);

  //Draw the little box
  x=wcheckbox->x+(strlen(wcheckbox->title)+2)*(wcheckbox->height-8);
  y=wcheckbox->y+4;
  
  glBegin(GL_LINES);
  glVertex2d(x, y);
  glVertex2d(x+wcheckbox->height-4, y );
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(x+wcheckbox->height-4, y );
  glVertex2d(x+wcheckbox->height-4, y+wcheckbox->height-4 );
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(x+wcheckbox->height-4, y+wcheckbox->height-4 );
  glVertex2d(x, y+wcheckbox->height-4 );
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(x, y+wcheckbox->height-4 );
  glVertex2d(x, y);
  glEnd();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv(color);
  
  
  glBegin(GL_QUADS); 
  glVertex3f(x, y, 0.0f);   //top left
  glVertex3f(x+wcheckbox->height-4, y, 0.0f);   //top right
  glVertex3f(x+wcheckbox->height-4, y+wcheckbox->height-4, 0.0f);  //Bottom right
  glVertex3f(x, y+wcheckbox->height-4, 0.0f);  //Bottom left  
  glEnd();
  glDisable(GL_BLEND);

  //Draw tick if checked
  if( wcheckbox->checked )
    {
      glColor3f(.2, .7, .2);
      x+=(wcheckbox->height-4)/2;
      y+=(wcheckbox->height-4)/2;

      glLineWidth(4.0f);
      glBegin(GL_LINES);
      glVertex2d(x-4, y+4 );
      glVertex2d(x, y);
      glEnd();
      
      glBegin(GL_LINES);
      glVertex2d(x, y );
      glVertex2d(x+8, y+8);
      glEnd(); 
      
      glLineWidth(1.0f);
    }
  
}

static int
ptInBox(Wcheckbox *wcheckbox, Wpoint mousexy)
{
  int x, y, size;

  x=wcheckbox->x+(strlen(wcheckbox->title)+2)*(wcheckbox->height-8);
  y=wcheckbox->y+4;
  size=wcheckbox->height-4;

  return (( mousexy.h < x+size || mousexy.h > x ) &&
	  ( mousexy.v < y+size || mousexy.v > y ));

}

void
mouse_wcheckbox( Wcheckbox *wcheckbox, int buttons, int state, int dblClick, Wpoint mousexy )
{
  if( buttons != 1 || state != SDL_PRESSED )
    return;

  if( ptInBox(wcheckbox, mousexy) )
    {
      wcheckbox->checked = ! wcheckbox->checked;
      if( wcheckbox->action != NULL )
	wcheckbox->action(wcheckbox);
    }
}


void
free_wcheckbox( Wcheckbox *wcheckbox )
{
  free(wcheckbox);
}



void
set_wcheckbox(Wcheckbox *wcheckbox, int checked )
{
  wcheckbox->checked = checked;
}


int
get_wcheckbox(Wcheckbox *wcheckbox )
{
  return wcheckbox->checked;
}

Wrect
getRect_wcheckbox(Wcheckbox *wcheckbox )
{
  Wrect rect;

  rect.top    = wcheckbox->y + wcheckbox->height;
  rect.left   = wcheckbox->x;
  rect.bottom = wcheckbox->y;
  rect.right  = wcheckbox->x + wcheckbox->width;

  return rect;
}
