#include "gltron.h"


Wlist *
new_wlist( int x, int y, int width, int height, int nblines, int nbcols, char *titles[] )
{
  Wlist *wlist;
  int    i;

  //Allocate memory
  wlist = ( Wlist *) malloc( sizeof(Wlist));

  //Inits
  wlist->x=x;
  wlist->y=y;
  wlist->width=width;
  wlist->height=height;
  wlist->nblines=nblines;
  wlist->nbcols=nbcols;
  wlist->current=-1;//means no one selected
  wlist->rlines=0;
  wlist->scroll=0;

  //Titles
  wlist->titles = (char **) malloc(sizeof(char *)*nbcols);
  for(i=0; i<nbcols; ++i)
    {
      wlist->titles[i] = (char *) malloc(sizeof(char)*strlen(titles[i])+1);
      strcpy(wlist->titles[i], titles[i]);
    }

  //lines
  wlist->lines=(char***)malloc(sizeof(char*)*nblines);
  for(i=0; i<nblines; ++i)
    {
      wlist->lines[i]=NULL;//(char **)malloc(sizeof(char *)*nbcols);
    }

  return wlist;
}


void
draw_wlist(Wlist *wlist, Trackerslots *slots)
{
  int h = wlist->height/wlist->nblines;
  int c = wlist->width/wlist->nbcols;
  int i, j;
  int x, y, s;
  glColor3f(1.0, 1.0, 1.0);

  //Draw Borders
  glBegin(GL_LINES);
  glVertex2d(wlist->x,                wlist->y);
  glVertex2d(wlist->x+wlist->width,   wlist->y);
  glEnd();

 
  glBegin(GL_LINES);
  glVertex2d(wlist->x+wlist->width,   wlist->y);
  glVertex2d(wlist->x+wlist->width,   wlist->y+wlist->height);
  glEnd();

  glBegin(GL_LINES);
  glVertex2d(wlist->x+wlist->width,   wlist->y+wlist->height);
  glVertex2d(wlist->x,   wlist->y+wlist->height);
  glEnd();


  glBegin(GL_LINES);
  glVertex2d(wlist->x,   wlist->y+wlist->height );
  glVertex2d(wlist->x,  wlist->y );
  glEnd();

  //draw Titles
  glBegin(GL_LINES);
  glVertex2d(wlist->x,               wlist->y+wlist->height-h );
  glVertex2d(wlist->x+wlist->width,  wlist->y+wlist->height-h );
  glEnd();
  

  y = wlist->y+wlist->height-h/2;
  s = (wlist->width/wlist->height)*8;
  for(i=0; i < wlist->nbcols; ++i)
    {
      x = wlist->x+i*c+10;
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINES);
      glVertex2d(x-10,  y-h/2 );
      glVertex2d(x-10,  y+h/2 );
      glEnd();
      glColor3f(1.0, 0.0, 0.0);
      drawText(netFtx, x, y, s, wlist->titles[i]);
    }

  glColor3f(0.4, 1.0, 0.4);

  //draw Lines
  for(i=wlist->scroll; i < wlist->scroll+wlist->nblines; ++i)
    {
      y = wlist->y+wlist->height-(i+1)*h-h/2;
      if( wlist->lines[i] != NULL )
	{
	  if( slots[i].packets==0 || (slots[i].ping/slots[i].packets > 400) || (strcmp(slots[i].version, VERSION) ))
	    {
	      if( i == wlist->current )
		glColor3f(0.9, 0.9, 0.9);
	      else
		glColor3f(0.7, 0.7, .7);	    
	    } else {
	      if( i == wlist->current )
		glColor3f(0.4, 0.4, 1.0);
	      else
		glColor3f(0.4, 1.0, 0.4);
	    }
	  for(j=0; j< wlist->nbcols; ++j)
	    {
	      x = wlist->x+j*c+10;
	      
	      drawText(netFtx, x, y, s, wlist->lines[i][j]);
	    }
	  glColor3f(1.0, 1.0, 1.0);
	  glBegin(GL_LINES);
	  glVertex2d(wlist->x,  y-h/2 );
	  glVertex2d(wlist->x+wlist->width,  y-h/2 );
	  glEnd();
	  glColor3f(1.0, 0.0, 0.0);
	}
    }
}

void
newline_wlist(Wlist *wlist, char **cols)
{
  int i;
  wlist->lines[wlist->rlines++] = (char **) malloc( wlist->nbcols * sizeof(char *) );
  for(i=0; i<wlist->nbcols; ++i)
    {
      wlist->lines[wlist->rlines-1][i]=(char *)malloc(strlen(cols[i]));
      strcpy(wlist->lines[wlist->rlines-1][i], cols[i]);
    }
}

void
updatecell_wlist( Wlist *wlist, char *val, int l, int c)
{
  free(wlist->lines[l][c]);
  wlist->lines[l][c]=(char *)malloc(strlen(val));
  strcpy(wlist->lines[l][c], val);
}
      

void
scroll_wlist(Wlist *wlist, int dir)
{
  switch( dir )
    {
    case LIST_SCROLL_UP:
      if( wlist->current == -1 )
	wlist->current = wlist->rlines;
      else if( wlist->current != 0 )
	wlist->current--;
      if( wlist->current < wlist->scroll )
	wlist->scroll--;
      break;
    case LIST_SCROLL_DOWN:
      if( wlist->current != wlist->rlines-1 )
	wlist->current++;
      if( wlist->current > wlist->scroll+wlist->nblines )
	wlist->scroll++;
      break;
    }
  printf("current %d\n",wlist->current); 
}

char   *
getcell_wlist ( Wlist *wlist, int l, int c )
{
  if( l < 0 || l > wlist->rlines )
    return NULL;
  if( c < 0 || c > wlist->nbcols )
    return NULL;
  return wlist->lines[l][c];
}

int
getcurrent_wlist(  Wlist *wlist )
{
  return wlist->current;
}


void
free_wlist(  Wlist *wlist )
{
  int i, j;

  //free lines
  for(i=0; i < wlist->nblines; ++i)
    {
      if( wlist->lines[i] != NULL )
	{
	  for(j=0; j<wlist->nbcols; ++j)
	    {
	      free(wlist->lines[i][j]);	     
	      wlist->lines[i][j]=NULL;
	    }
	  free(wlist->lines[i]);
	  wlist->lines[i]=NULL;
	}
    }
  free(wlist->lines);
  wlist->lines=NULL;

  //free titles
  for(i=0; i<wlist->nbcols; ++i)
    {
      free(wlist->titles[i]);
      wlist->titles[i]=NULL;
    }
  free(wlist->titles);

  free(wlist);
}
