#include "gltron.h"

#define MAXWLISTLINES 2560

/** Columns Definitions */
ColDef*
new_colDefs(int nbcols)
{
  ColDef *colDefs;
  int     i;
  
  //Allocate memory
  colDefs = ( ColDef *) malloc(sizeof(ColDef)*nbcols);

  for(i=0; i<nbcols; ++i)
    {
      colDefs[i].title   = NULL;
      colDefs[i].colsize = 0;
      colDefs[i].color[0] = 1.0;
      colDefs[i].color[1] = 1.0;
      colDefs[i].color[2] = 1.0;
      colDefs[i].callbacks.drawit=NULL;
      colDefs[i].callbacks.sortit=NULL;
      colDefs[i].callbacks.tostr=NULL;
    }

  return colDefs;
}

void
set_colDef( ColDef *colDefs, int col, char *title, int colsize,
	    float color[3],   
	    void  (*drawit)( WlistPtr list, int x, int y, int line, int col ),
	    char* (*tostr)(WlistPtr list, int line, int col), 
	    int   (*sortit)( WlistPtr list, int line, int next ))
{
  colDefs[col].title = (char *)malloc(strlen(title)+1);
  strcpy(colDefs[col].title, title);
  colDefs[col].colsize=colsize;
  colDefs[col].color[0] = color[0];
  colDefs[col].color[1] = color[1];
  colDefs[col].color[2] = color[2];
  colDefs[col].callbacks.drawit = drawit;
  colDefs[col].callbacks.tostr  = tostr;
  colDefs[col].callbacks.sortit = sortit;
}


/** List stuffs */

Wlist *
new_wlist(  int x, int y, int width, int height, int nblines, int nbcols,
	    ColDef *colDefs, int sortcol, 
	    void  (*focus) ( WlistPtr list, int line ) )
{
  Wlist *wlist = NULL;


  if( colDefs == NULL )
    return NULL;

  //Allocate memory
  wlist = (Wlist *) malloc(sizeof(Wlist));

  //Inits
  wlist->x=x;
  wlist->y=y;
  wlist->width=width;
  wlist->height=height;
  wlist->nblines=nblines;
  wlist->nbcols=nbcols;
  wlist->current=-1;
  wlist->rlines=0;
  wlist->scroll=0;
  wlist->colDefs=colDefs;
  wlist->sortcol=sortcol;
  wlist->focus  = focus;
  
  //Init and allocte memory for index and datas
  wlist->index = (int *)malloc(sizeof(int)*MAXWLISTLINES);
  initindex_wlist(wlist);
  wlist->lines = (char ***)malloc(sizeof(char **) * MAXWLISTLINES);
  return wlist;
}

void
draw_wlist( Wlist *wlist )
{
  int h = wlist->height/wlist->nblines;
  int c = wlist->width/wlist->nbcols;
  int x, y, s, i, j;


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
  s = h-10;

  for( i = 0; i < wlist->nbcols; ++i )
    {
      x = wlist->x+i*c+10;
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINES);
      glVertex2d(x-10,  y-h/2 );
      glVertex2d(x-10,  y+h/2 );
      glEnd();
      drawText(netFtx, x, y-h/2+4, s, wlist->colDefs[i].title);
    }

  s-=5;
  //draw Lines
  for(i=wlist->scroll; i < wlist->scroll+wlist->nblines; ++i )
    {
      y = wlist->y+wlist->height-(i+1)*h-h/2;
      if( wlist->lines[i] != NULL )
	{
	  //draw columns
	  for( j=0; j < wlist->nbcols; ++j)
	    {
	      x=wlist->x+j*c+10;
	      if( wlist->colDefs[j].callbacks.drawit != NULL )
		wlist->colDefs[j].callbacks.drawit(wlist, x, y, wlist->index[i], j);

	      //color!!
	      if( i == wlist->current )
		{
		  //draw hilight
		  glColor3f(.1, .1, 0.4);
		  glBegin(GL_QUADS);	  
		  glVertex3f(x-9, y+h/2-1, 0.0f);     //top left
		  if( j == wlist->nbcols-1 )
		    {
		      glVertex3f(x+c-11, y+h/2-1, 0.0f);  //top right
		      glVertex3f(x+c-11, y-h/2+1, 0.0f);  //Bottom right
		    } else {
		      glVertex3f(x+c-9, y+h/2-1, 0.0f);   //top right
		      glVertex3f(x+c-9, y-h/2+1, 0.0f);   //Bottom right
		    }
		  glVertex3f(x-9, y-h/2+1, 0.0f);         //Bottom left
		  glEnd();
		  glColor3f(.9, .9, 0.6);
		} else 
		  glColor3f(wlist->colDefs[j].color[0], wlist->colDefs[j].color[1], wlist->colDefs[j].color[2] );

	      if( wlist->colDefs[j].callbacks.tostr != NULL )
		drawText(netFtx, x, y-h/2+7, s, wlist->colDefs[j].callbacks.tostr(wlist,wlist->index[i], j));

	    }
	  glColor3f(1.0, 1.0, 1.0);
	  glBegin(GL_LINES);
	  glVertex2d(wlist->x,  y-h/2 );
	  glVertex2d(wlist->x+wlist->width,  y-h/2 );
	  glEnd();
	}
    }
}

void
setCell_wlist    ( Wlist *wlist, char *data, int len, int line, int col )
{
  char **theLine;
  int test = (int)*data;

  theLine = wlist->lines[line];

  if( (theLine == NULL ) || ( line > MAXWLISTLINES ) || ( col > wlist->nbcols ))
    return;
  
  fprintf(stderr, "setting line %d col %d\n", line, col);
  if( wlist->lines[line][col] != NULL )
    {
      free(wlist->lines[line][col]);
      wlist->lines[line][col]=NULL;
    }
  printf("setting %d\n", test);
  wlist->lines[line][col] = ( char *) malloc(len);
  memcpy(wlist->lines[line][col], data, len);
  rebuildindex_wlist(wlist);
}

char *
getCell_wlist    ( Wlist *wlist,  int line, int col)
{
  return wlist->lines[line][col];
}

int
addRow_wlist     ( Wlist *wlist, short nbrows )
{
  int i;
  int j;

  for(i=0; i< nbrows; ++i)
    {
      if( wlist->rlines >= MAXWLISTLINES )
	return -1;

      if( wlist->lines[wlist->rlines] == NULL )
      {
	wlist->lines[wlist->rlines]=(char **)malloc(sizeof(char*)*wlist->nbcols);
	wlist->rlines++;
	fprintf(stderr, "--> adding a line, rlines=%d\n", wlist->rlines);
	for(j=0; j<wlist->nbcols; ++j)
	  {
	    wlist->lines[wlist->rlines-1][j]=NULL;
	  }
      }
    }
  return wlist->rlines-nbrows;
}

void
rebuildindex_wlist(Wlist *wlist)
{
  int i;
  int sorted=0;
  int tmp;

  //while(!sorted) { sorted = 1; for(i = 0; i < n - 1; i++) if(data[i] > data[i + 1]) { swap(i, i+1); sorted = 0; } }
  while( ! sorted )
    {
      sorted = 1;
      for(i=0; i < wlist->rlines-1; ++i)
	{
	  if(( wlist->colDefs[wlist->sortcol].callbacks.sortit != NULL )
	     && ( wlist->lines[i+1][wlist->sortcol]!= NULL) )
	    {
	      //wlist->colDefs[j].callbacks.tostr
	      printf("sorting %d <-> %d ( rlines %d )\n", wlist->index[i], wlist->index[i+1], wlist->rlines);
	      if( wlist->colDefs[wlist->sortcol].callbacks.sortit(wlist, wlist->index[i], wlist->index[i+1]) == 1)
		{
		  tmp=wlist->index[i+1];
		  wlist->index[i+1]=wlist->index[i];
		  wlist->index[i]=tmp;
		  fprintf(stderr, "swaping %d [ %d ] with %d [ %d ]\n", i, wlist->index[i], i+1, wlist->index[i+1]);
		  sorted = 0;
		}
	    }
	}
    }
}

void
initindex_wlist(Wlist *wlist)
{
  int i;
  
  for(i=0; i< MAXWLISTLINES; ++i)
    {
      wlist->index[i]=i;
    }
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
  if( wlist->focus != NULL )
    wlist->focus(wlist, wlist->current);
  printf("current %d\n",wlist->current); 
}
int
getcurrent_wlist( Wlist *wlist )
{
  return wlist->current;
}

void
free_wlist       ( Wlist *wlist )
{
  int i, j;

  //free lines
  for(i=0; i < MAXWLISTLINES; ++i)
    {
      if( wlist->lines[i] != NULL )
	{
	  for(j=0; j<wlist->nbcols; ++j )
	    {
	      free(wlist->lines[i][j]);
	      wlist->lines[i][j]=NULL;
	    }
	  free( wlist->lines[i]);
	  wlist->lines[i]=NULL;
	}
    }
  free(wlist->lines);
  wlist->lines=NULL;
  free(wlist->index);
  wlist->index=NULL;

  
  free(wlist);
}
