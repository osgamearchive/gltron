#include "gltron.h"

Wtext *
new_wtext(int width, int height, int posx, int posy, int nblines)
{
  Wtext *wtext;
  int    i;

  wtext = ( Wtext * ) malloc( sizeof( Wtext ));

  wtext->width       = width;
  wtext->height      = height;
  wtext->nblines     = nblines;
  wtext->nbchars     = wtext->width*wtext->nblines/wtext->height*1.5;
  wtext->nbchars     +=2;
  printf("nbchars = %d\n", wtext->nbchars);
  wtext->x           = posx;//-2*game->screen->vp_w / (50 * 1.5);
  wtext->y           = posy;

  wtext->current     = 0;
  wtext->cur_char    = 0;

  wtext->buffer      = ( char ** ) malloc( wtext->nblines * sizeof(char *) );

  //put blanck text for each line.
  for( i=0; i< wtext->nblines; ++i )
    {
      wtext->buffer[i] = ( char * ) malloc( wtext->nbchars+1 );
      wtext->buffer[i][0] = '\0';
      printf("line %d is %s\n", i, wtext->buffer[i]);
    }
 
  return wtext;
}


void
scroll_wtext(Wtext *wtext, int size)
{
  int i;
  printf("scroll %d lines\n", size);
  for(i=0; i< wtext->nblines-size; ++i)
    {
      strcpy(wtext->buffer[i], wtext->buffer[i+size]);
    }

  for(i=wtext->current; i < wtext->nblines; ++i)
    {
      wtext->buffer[i][0]='\0';
    }
}

void
insert_wtext(Wtext *wtext, char *text, int color_code)
{
  int i=0, j=0;

  if( wtext == NULL || text == NULL )
      return;

  color_code+=48;
  //Add color code
  //wtext->buffer[wtext->current][i++]=3;
  //wtext->buffer[wtext->current][i++]=color_code;

  i = wtext->cur_char;

  //Copy text in lines
  while( i < wtext->nbchars && text[j] != 0 )
    {
      if( i == 0 )
	{
	  wtext->buffer[wtext->current][i++]=3;
	  wtext->buffer[wtext->current][i++]=color_code;
	}
      if( text[j] == '\n' || text[j] == '\r' )
	{
	  printf("get a LR or a CR, end at %d\n", i);
	  wtext->buffer[wtext->current][i]='\0';
	  printf("changing line: %s\n first is %s\n", wtext->buffer[wtext->current], wtext->buffer[0]);
	  if( wtext->current >= (wtext->nblines-1) )
	    {
	      printf("need to scroll current = %d is > %d\n", wtext->current, wtext->height);
	      scroll_wtext(wtext, 1);
	    } else {
	      wtext->current++;
	    }
	  j++;
	  i=0;
	  /* wtext->buffer[wtext->current][i++]=3; */
/* 	  wtext->buffer[wtext->current][i++]=color_code; */
	} else {	  
	  if( i >= (wtext->nbchars-1) )
	    {
	      printf("changing line: %s\n", wtext->buffer[wtext->current]);
	      wtext->buffer[wtext->current][i]='\0';
	      if( wtext->current >= (wtext->nblines-1) )
		{
		  printf("need to scroll current = %d is > %d\n", wtext->current, wtext->height);
		  scroll_wtext(wtext, 1);
		} else { 
		  wtext->current++;
		}
	      i=0;
	      /* wtext->buffer[wtext->current][i++]=3; */
/* 	      wtext->buffer[wtext->current][i++]=color_code; */
	    } else {
	      printf("adding to ligne %d char %d in pos %d\n", wtext->current, text[j], i);
	      wtext->buffer[wtext->current][i] = text[j++];
	      i++;
	    }
	}
    }
  if( i == 2 )
    {
      i = 0;
    }
  wtext->buffer[wtext->current][i]='\0';
  wtext->cur_char=i;
  printf("end of string at %d line %d %s\n", i, wtext->current, wtext->buffer[0]);
}

void
draw_wtext( Wtext *wtext )
{
  int i;
  int h;
  int y;
  int x;


  if( wtext == NULL )
    return;

  
  h = wtext->height / ( wtext->nblines * 1.5 );
  //printf("h = %d ( %d )\n", h,wtext->height );
  //h = game->screen->vp_h / (24 * 1.5);
  x = wtext->x;

  //printf("displaying line 0 to %d\n", wtext->height);
  for(i=0; i < wtext->nblines; ++i )
    {
      y = wtext->y - 1.5 * h * i ;
      if( wtext->buffer[i][0] == '\0' ) //Why is this necessary?? !!!
	return;
      //printf("line %d -> %s\n", i, wtext->buffer[i]);
      //ftxRenderString(gameFtx, wtext->buffer[i], strlen(wtext->buffer[i]));
      //printf("drawing at pos %d\n", wtext->x);
      //printf("x= %d y = %d\n", x, y );
      drawText(gameFtx, x, y, h, wtext->buffer[i]);   
    }
}

void
free_wtext(Wtext *wtext)
{
  int i;
  for( i=0; i< wtext->nblines; ++i )
    {
      free(wtext->buffer[i]);
    }
  free(wtext);
  wtext=NULL;
}

Wintext *
new_wintext ( int width, int height, int posx, int posy, int nbchars, int maxchars )
{
  Wintext *wintext;

  wintext = ( Wintext * ) malloc( sizeof( Wintext ));

  wintext->width       = width;
  wintext->height      = height;
  wintext->nbchars     = nbchars;
  wintext->maxchars    = maxchars;
  wintext->x           = posx;
  wintext->y           = posy;

  wintext->time        = 0;  
  wintext->offset      = SystemGetElapsedTime();

  wintext->cur_char    = 0;

  wintext->buffer      = ( char * ) malloc( wintext->maxchars + 2 );

  wintext->buffer[0] = '\0';

  return wintext;
}

void
free_wintext(Wintext *wintext)
{
  free(wintext->buffer);
  free(wintext);
  wintext=NULL;
}

void
key_wintext(Wintext *wintext, int charcode, int unicode)
{
  if( wintext->cur_char > wintext->maxchars )
    return;

  /* if( unicode < 0x80 && unicode > 0 ) */
/*     { */
      

      switch( charcode )
	{
	case SDLK_BACKSPACE:
	  printf("backspace");
	  break;
	case SDLK_DELETE:
	  printf("delete");
	  break;
	case SDLK_UP:
	  printf("going up");
	  break;
        case SDLK_DOWN:
	  printf("going down");
	  break;
        case SDLK_RIGHT:
	  printf("going right");
	  //wintext->cur_char++;
	  break;
        case SDLK_LEFT:
	  printf("going left");
	  //wintext->cur_char--;
	  break;
	default:
	  if( unicode < 0x80 && unicode > 0 ) 
	    { 
	      wintext->buffer[wintext->cur_char++]=(char)unicode;
	      wintext->buffer[wintext->cur_char]='\0';
	    }
	  break;
	}
      //}
}

void
draw_wintext(Wintext *wintext)
{
  int   h;
  int   y;
  int   x;
  int   start, end;
  char *tmp;


  if( wintext == NULL )
    return;

  
  h = (int) ((float)wintext->width /(float) wintext->nbchars-1);

  x = wintext->x;
  y = wintext->y;

  //moving window into the buffer
  tmp = (char *) malloc(wintext->nbchars+2);
  strcpy(tmp, "");
  if( wintext->cur_char <= wintext->nbchars )
    {
      start=0;
      end=strlen(wintext->buffer);
    } else {
      //start=wintext->cur_char-wintext->nbchars;
      start=strlen(wintext->buffer)-wintext->nbchars;
      end=wintext->nbchars;
    }
  
  //printf("start %d end %d\n", start, end);
  strncpy(tmp, &wintext->buffer[start], end);
  tmp[end]='\0';
  //printf("drawing %s\n", wintext->buffer);
  drawText(gameFtx, x, y, h, tmp);   

  wintext->time = SystemGetElapsedTime() - wintext->offset;
  if( wintext->time % 1000 > 200 )
    {
      //no line
      //do nothing
    } else {
      //a line
      glLineWidth(2.0f);
      glBegin(GL_LINES);
      x+= end*h;
      glVertex2d(x,y);	// Left Side Of Horizontal Line
      glVertex2d(x,y+h);	// Right Side Of Horizontal Line
      glEnd();	


    }

}

char *
get_wintext(Wintext *wintext)
{
  char *text;

  text = (char *)malloc(strlen(wintext->buffer)+1);
  strcpy(text, wintext->buffer);
  return text;
}

void
clear_wintext(Wintext *wintext)
{
  strcpy(wintext->buffer, "");
  wintext->cur_char    = 0;
}
