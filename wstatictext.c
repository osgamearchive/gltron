#include "gltron.h"


Wstatictext *
new_wstatictext( int x, int y, int width, int height, char *text,
			      int size, int font, float color[3])
{
  Wstatictext *wstatictext=NULL;

  if( text == NULL )
    return NULL;

 
  wstatictext=(Wstatictext *)malloc(sizeof(Wstatictext));

  wstatictext->x=x;
  wstatictext->y=y;
  wstatictext->width=width;
  wstatictext->height=height;
  wstatictext->size=size;
  wstatictext->font=font;
  wstatictext->color[0]=color[0];
  wstatictext->color[1]=color[1];
  wstatictext->color[2]=color[2];
  wstatictext->text=(char *)malloc(strlen(text)+1);
  strcpy(wstatictext->text, text);

  return wstatictext;
}


void
draw_wstatictext(Wstatictext *wstatictext)
{
  fonttex *theFont=NULL;
  int      maxchar;
  char    *str;
  int      x;
  int      y;

  glColor3fv(wstatictext->color);

  switch( wstatictext->font )
    {
    case guiFont:
      theFont=guiFtx;
      break;
    case gameFont:
      theFont=gameFtx;
      break;
    case netFont:
      theFont=netFtx;
    }
  if( theFont == NULL || wstatictext->text == NULL )
    return;

  if( wstatictext->size > wstatictext->height )
    maxchar = 0;
  else
    maxchar =  wstatictext->width*1.5*wstatictext->size;

  str = (char *)malloc(maxchar+1);
  strncpy(str, wstatictext->text, maxchar);

  //draw middle of the box
  x = wstatictext->x + wstatictext->width/2 - wstatictext->size*strlen(str)/2;
  y = wstatictext->y + (wstatictext->height-wstatictext->size)/2;

  drawText(theFont, x, y, wstatictext->size, str);
  free(str);
}


void
set_wstatictext(Wstatictext *wstatictext, char *ntext)
{
  free(wstatictext->text);
  wstatictext->text=(char *)malloc(strlen(ntext)+1);
  strcpy(wstatictext->text, ntext);
}


void
free_wstatictext(Wstatictext *wstatictext)
{
  free(wstatictext->text);
  wstatictext->text=NULL;
  free(wstatictext);
}

Wrect
getRect_wstatictext( Wstatictext *wstatictext )
{
  Wrect rect;

  rect.top    = wstatictext->y + wstatictext->height;
  rect.left   = wstatictext->x;
  rect.bottom = wstatictext->y;
  rect.right  = wstatictext->x + wstatictext->width;

  return rect;
}
