#include "gltron.h"

static void up_wintext( Wintext *wintext );
static void down_wintext( Wintext *wintext );
static int history_length_wintext(Wintext *wintext);

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
  wintext->wstart      = 0;  
  wintext->wend        = 0;//wintext->maxchars;

  wintext->buffer      = ( char * ) malloc( wintext->maxchars + 2 );

  wintext->buffer[0] = '\0';

  wintext->history = NULL;
  wintext->h_num = 0;

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
  int i;
  if( strlen(wintext->buffer) >= wintext->maxchars )
    return;

  /* if( unicode < 0x80 && unicode > 0 ) */
/*     { */
      

      switch( charcode )
	{
	case SDLK_BACKSPACE:
	  printf("backspace\n");
	  if(wintext->cur_char == 0 )
	    return;
	  wintext->cur_char--;
	  for(i=wintext->cur_char; i < strlen(wintext->buffer) ; i++)
	    {
	      wintext->buffer[i]=wintext->buffer[i+1];
	    }
	  break;
	case SDLK_DELETE:
	  printf("delete\n");
	  if(wintext->cur_char == 0 )
	    return;
	  for(i=wintext->cur_char; i < strlen(wintext->buffer) ; i++)
	    {
	      wintext->buffer[i]=wintext->buffer[i+1];
	    }
	  break;
	case SDLK_UP:
	  printf("going up\n");
	  up_wintext(wintext);
	  break;
        case SDLK_DOWN:
	  printf("going down\n");
	  down_wintext(wintext);
	  break;
        case SDLK_RIGHT:
	  printf("going right\n");
	  if( wintext->cur_char == strlen(wintext->buffer) )
	    return;
	  wintext->cur_char++;
	  if( ( wintext->cur_char < wintext->wstart ) &&(  wintext->wstart > 0))
	    {
	      wintext->wstart++;
	      wintext->wend++;
	    }
	  break;
        case SDLK_LEFT:
	  printf("going left\n");
	  if(wintext->cur_char == 0 )
	    return;
	  wintext->cur_char--;
	  if( ( wintext->cur_char > wintext->wend ) &&(  wintext->wend < strlen(wintext->buffer)))
	    {
	      wintext->wstart--;
	      wintext->wend--;
	    }
	  break;
	default:
	  if( unicode < 0x80 && unicode > 0 ) 
	    {
	      printf(" wend:%d == len:%d ?\n", wintext->wend, strlen(wintext->buffer));
	      if( wintext->wend == strlen(wintext->buffer) && strlen(wintext->buffer) >= wintext->nbchars  )
		{		
		  wintext->wstart++;
		}
	      printf(" len:%d == nbchars:%d !\n", strlen(wintext->buffer), wintext->nbchars);
	      
	      if( wintext->wend == strlen(wintext->buffer))
		{	
		  wintext->wend++;
		}/*  else { */
/* 		  if( strlen(wintext->buffer) == strlen(wintext->buffer) ) */
/* 		    { */
/* 		      wintext->wend        = wintext->wstart+wintext->maxchars; */
/* 		    } */
/* 		} */
	      printf("start %d, end:%d, curchar:%d\n", wintext->wstart, wintext->wend,wintext->cur_char );
	      if( strlen(wintext->buffer) == wintext->cur_char)
		{
		  wintext->buffer[wintext->cur_char++]=(char)unicode;
		  wintext->buffer[wintext->cur_char]='\0';
		} else {
		  wintext->buffer[strlen(wintext->buffer)+1]='\0';
		  for(i=strlen(wintext->buffer); i >= wintext->cur_char ; i--)
		    {
		    wintext->buffer[i]=wintext->buffer[i-1];
		    }
		  wintext->buffer[wintext->cur_char++]=(char)unicode;
		  if( wintext->wend < wintext->nbchars )
 		    wintext->wend++;
		  /* else */
/* 		    wintext->wend--; */
/* 		  if(wintext->cur_char==wintext->wend) */
/* 		    { */
/* 		      wintext->wend+=2; */
/* 		    } */
		  /* if( wintext->wstart > 0 && wintext->wend < wintext->nbchars) */
/* 		    wintext->wstart--; */
/* 		  if( wintext->wend < wintext->nbchars ) */
/* 		    wintext->wend++; */
		  
		}
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
  int   len;
  char *tmp;


  if( wintext == NULL )
    return;

  
  h = (int) ((float)wintext->width /(float) wintext->nbchars-1);

  x = wintext->x;
  y = wintext->y;

  //moving window into the buffer
  tmp = (char *) malloc(wintext->nbchars+1);
  if(tmp==NULL)
    return;
  strcpy(tmp, "");
  /* if( wintext->cur_char <= wintext->nbchars ) */
/*     { */
/*       start=0; */
/*       end=strlen(wintext->buffer); */
/*     } else { */
/*       //start=wintext->cur_char-wintext->nbchars; */
/*       start=strlen(wintext->buffer)-wintext->nbchars; */
/*       end=wintext->nbchars; */
/*     } */
  
    // printf("start %d end %d\n", wintext->wstart, wintext->wend);
    len = +wintext->wend-wintext->wstart;
    /* if( len > wintext->nbchars) */
/*       { */
/* 	len = wintext->nbchars; */
/* 	error */

/*       } */
    
  strncpy(tmp, &wintext->buffer[wintext->wstart], len);
  tmp[len]='\0';
  //printf("drawing %s\n", tmp);
  drawText(gameFtx, x, y, h, tmp);   

  wintext->time = SystemGetElapsedTime() - wintext->offset;
  if( wintext->time % 1000 > 500 )
    {
      //no line
      //do nothing
    } else {
      //a line
      glLineWidth(2.0f);
      glBegin(GL_LINES);
      len=(wintext->cur_char-wintext->wstart);
      if( len < 0 )
	len =0;
      if( len > wintext->nbchars )
	len = wintext->nbchars;
      x+= len*h - 1;
      glVertex2d(x,y);	        // Left Side Of Horizontal Line
      glVertex2d(x,y+h);	// Right Side Of Horizontal Line
      glEnd();	
      glLineWidth(1.0f);
    }
  free(tmp);
  tmp=NULL;
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
  History  history;
  if( strlen(wintext->buffer) == 0 )
    return;
  history = ( History ) malloc( sizeof(CHistory));
  if( history != NULL )
    {
      history->next = wintext->history;
      history->text = ( char *) malloc( strlen(wintext->buffer)+1);
      strcpy(history->text, wintext->buffer);

      wintext->history=history;      
      wintext->h_num=history_length_wintext(wintext)+1;
      printf("clear -> num=%d\n", wintext->h_num);
    }

  strcpy(wintext->buffer, "");
  wintext->cur_char    = 0;
  wintext->wstart      = 0;
  wintext->wend        = 0;
}

void
up_wintext( Wintext *wintext )
{
  History history;
  int     num;
  //first clear the wintext
  num=wintext->h_num;
  if( num == history_length_wintext(wintext)+1)
    clear_wintext(wintext);
  wintext->h_num=num;
  if( wintext->h_num > 1 )
    {
      history = wintext->history;
      num = history_length_wintext(wintext)-wintext->h_num+1;
      printf("num %d size %d( %d )\n", num, history_length_wintext(wintext), wintext->h_num);
      while( history && (num-->0)) { history=history->next;}
      if( history == NULL )
	return;
      strcpy(wintext->buffer, history->text);
      wintext->h_num--;

      //adjust wstart and wend
      wintext->wstart=0;
      if( strlen(wintext->buffer) > wintext->nbchars )
	wintext->wend = wintext->nbchars;
      else
	wintext->wend = strlen(wintext->buffer);
    } else {      
      wintext->h_num=1;
    }
  
}
void
down_wintext( Wintext *wintext )
{
  History history;
  int     num;

  //first clear the wintext
/*   num=wintext->h_num; */
/*   if( num == history_length_wintext(wintext)+1) */
/*     clear_wintext(wintext); */
/*   wintext->h_num=num; */
  if( wintext->h_num < history_length_wintext(wintext) )
    {
      history = wintext->history;
      num = history_length_wintext(wintext)-wintext->h_num;
      printf("num %d size %d( %d )\n", num, history_length_wintext(wintext), wintext->h_num);
      while( history && (--num>0)) { history=history->next;}
      if( history == NULL )
	return;
      strcpy(wintext->buffer, history->text);
      wintext->h_num++;

      //adjust wstart and wend
      wintext->wstart=0;
      if( strlen(wintext->buffer) > wintext->nbchars )
	wintext->wend = wintext->nbchars;
      else
	wintext->wend = strlen(wintext->buffer);
    } else {
      printf("came to first entry\n");
      strcpy(wintext->buffer, "");
      wintext->cur_char    = 0;
      wintext->wstart      = 0;
      wintext->wend        = 0;
      wintext->h_num       = history_length_wintext(wintext);
    }
}

int
history_length_wintext(Wintext *wintext)
{
  int      size = 0;
  History  history;

  history = wintext->history;
  while( history ) { size++; history=history->next; }

  return size;
}
