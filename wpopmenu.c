#include "gltron.h"

Wpopmenu*
new_wpopmenu(int x, int y, int width, int height, char *title,
	     void (*action)(Wpopmenu *wpopmenu))
{
  Wpopmenu  *wpopmenu = NULL;

  wpopmenu = ( Wpopmenu  *)malloc(sizeof(Wpopmenu));

  wpopmenu->width=width;
  wpopmenu->height=height;
  wpopmenu->x=x;
  wpopmenu->y=y;
  wpopmenu->nbMenus=0;
  wpopmenu->action=action;
  
  if( title != NULL )
    strncpy(wpopmenu->title, title, 254);
  else
    strcpy(wpopmenu->title, "");
  wpopmenu->selected=-1;
  wpopmenu->newselect=-1;
  wpopmenu->isOpen=0;
  wpopmenu->menuList=NULL;

  return wpopmenu;
}


void
addchoice_wpopmenu(Wpopmenu *wpopmenu, char *name, int value)
{
  Wmenu *menu;
  Wmenu *tmp;

  menu = ( Wmenu *)malloc(sizeof(Wmenu));

  menu->next = NULL;

  strncpy(menu->name, name, 254);
  menu->value = value;

  //add to menu List
  tmp = wpopmenu->menuList;
  if( tmp == NULL )
    {
      wpopmenu->menuList=menu;
    } else {
      while( tmp->next != NULL )
	{
	  tmp = tmp->next;
	}
      tmp->next = menu;
    }
  wpopmenu->nbMenus++;
}

void
select_wpopmenu(Wpopmenu *wpopmenu, int menuchoice )
{
  if( menuchoice < 0 || menuchoice > wpopmenu->nbMenus )
    return;
  wpopmenu->selected = menuchoice;
  wpopmenu->newselect = wpopmenu->selected;
}

int
getValue_wpopmenu(Wpopmenu *wpopmenu)
{
  int select = wpopmenu->newselect;
  int i =0;
  Wmenu *menu = wpopmenu->menuList;

  while( (menu != NULL) && (i++ < select) ) 
    {
      menu = menu->next;
    }
  return menu->value;
}

void
setTitle_wpopmenu(Wpopmenu *wpopmenu, char *title)
{
  if( title == NULL || strlen(title ) > 254 )
    return;
  strcpy(wpopmenu->title, title);
}

void
free_wpopmenu(Wpopmenu *wpopmenu)
{
  Wmenu *tmp, *aux;

  tmp = wpopmenu->menuList;
  while( tmp != NULL )
    {
      aux = tmp->next;
      free(tmp);
      tmp=aux;
    }
  free(wpopmenu);
}


static Wmenu *
getMenu(Wpopmenu *wpopmenu, int menuChoice)
{
  int i = 0;
  Wmenu *menu;

  //printf("search for menu %d\n", menuChoice);
  if( menuChoice < 0 || menuChoice > wpopmenu->nbMenus )
    return NULL;

  menu = wpopmenu->menuList;

  while( (menu != NULL) && (i++ < menuChoice) )
    {
      //printf("i=%d found menu named %s\n",i, menu->name);
      menu = menu->next;
    }
  return menu;
}

void
draw_wpopmenu(Wpopmenu *wpopmenu, int active)
{
  int x, y;
  int i=0;
  int s;
  Wmenu *menu;
  float color[4] = { 0.05f, 0.05f, 0.05f, .8f };
  //int h = game->screen->vp_w / 50;

  if( ! active )
    glColor3f(.5, .5, .5);
  else
    glColor3f(1.0, 1.0, 1.0);

  //If title, draw it
  if( strlen(wpopmenu->title) > 0 )
    {
      x=wpopmenu->x+10;
      y=wpopmenu->y+4;
      drawText(netFtx, x, y, wpopmenu->height-8, wpopmenu->title);
    }

  //Draw the closed box
  x=wpopmenu->x+(strlen(wpopmenu->title)+2)*(wpopmenu->height-8);
  y=wpopmenu->y;
  
  glBegin(GL_LINES);
  glVertex2d(x, y);
  glVertex2d(wpopmenu->x+wpopmenu->width, y );
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(wpopmenu->x+wpopmenu->width, y );
  glVertex2d(wpopmenu->x+wpopmenu->width, wpopmenu->y+wpopmenu->height );
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(wpopmenu->x+wpopmenu->width, wpopmenu->y+wpopmenu->height );
  glVertex2d(x, wpopmenu->y+wpopmenu->height );
  glEnd();
  
  
  glBegin(GL_LINES);
  glVertex2d(x, y+wpopmenu->height );
  glVertex2d(x, y);
  glEnd();

  //Draw the background
  if( wpopmenu->isOpen == 1 )
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4fv(color);

      
      glBegin(GL_QUADS); 
      glVertex3f(x, wpopmenu->y-wpopmenu->height*(wpopmenu->nbMenus-wpopmenu->selected), 0.0f);   //top left
      glVertex3f(wpopmenu->x+wpopmenu->width, wpopmenu->y-wpopmenu->height*(wpopmenu->nbMenus-wpopmenu->selected), 0.0f);   //top right
      glVertex3f(wpopmenu->x+wpopmenu->width, wpopmenu->y+wpopmenu->height*(wpopmenu->selected+1), 0.0f);  //Bottom right
      glVertex3f(x, wpopmenu->y+wpopmenu->height*(wpopmenu->selected+1), 0.0f);  //Bottom left  
      glEnd();
      glDisable(GL_BLEND);
    } else {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4fv(color);

      
      glBegin(GL_QUADS); 
      glVertex3f(x, y, 0.0f);   //top left
      glVertex3f(wpopmenu->x+wpopmenu->width, y, 0.0f);   //top right
      glVertex3f(wpopmenu->x+wpopmenu->width, y+wpopmenu->height, 0.0f);  //Bottom right
      glVertex3f(x, y+wpopmenu->height, 0.0f);  //Bottom left  
      glEnd();
      glDisable(GL_BLEND);
    }

  // s = wpopmenu->height-8;
  s = 8*wpopmenu->width/200;;

    glColor3f(1.0, 1.0, 1.0);
  //Draw current value
  menu = getMenu(wpopmenu, wpopmenu->selected);
  if( menu != NULL )
    {
      if( wpopmenu->selected == wpopmenu->selected && wpopmenu->isOpen != 1 )
	glColor3f(1.0, .1, .1);
      else if( wpopmenu->selected == wpopmenu->newselect && wpopmenu->isOpen == 1 )
	glColor3f(.1, 1.0, .1);
      drawText(gameFtx, x+10, y+4, s, menu->name);
    }

  glColor3f(1.0, 1.0, 1.0);
  //Draw the box
  if( wpopmenu->isOpen == 1 )
    {
      //Draw the open box
      for(i=0; i< wpopmenu->selected; ++i )
	{
	  if( i == 0 )
	    {
	       glBegin(GL_LINES);
	       glVertex2d(x, y+wpopmenu->height*(wpopmenu->selected-i+1) );
	       glVertex2d(wpopmenu->x+wpopmenu->width, y+wpopmenu->height*(wpopmenu->selected-i+1));
	       glEnd();
	    }
	  
	  glBegin(GL_LINES);
	  glVertex2d(x, y - wpopmenu->height * ( i - wpopmenu->selected) );
	  glVertex2d(x, y - wpopmenu->height * ( i - wpopmenu->selected - 1));
	  glEnd();
	  glBegin(GL_LINES);
	  glVertex2d(wpopmenu->x + wpopmenu->width, y - wpopmenu->height * ( i - wpopmenu->selected) );
	  glVertex2d(wpopmenu->x + wpopmenu->width, y - wpopmenu->height * ( i - wpopmenu->selected - 1));
	  glEnd();
	  menu = getMenu(wpopmenu, i);
	  if( menu != NULL )
	    {
	      if( i == wpopmenu->newselect )
		glColor3f(.1, 1.0, .1);
	      drawText(gameFtx, x+10, y-wpopmenu->height*(i-wpopmenu->selected)+4, s, menu->name);
	      glColor3f(1.0, 1.0, 1.0);
	    }
	}
      glColor3f(1.0, 1.0, 1.0);
      for(i=wpopmenu->selected+1; i< wpopmenu->nbMenus; ++i)
	{
	  if( i == wpopmenu->nbMenus-1 )
	    {
	      glBegin(GL_LINES);
	      glVertex2d(x, y - wpopmenu->height * ( i - wpopmenu->selected) );
	      glVertex2d(wpopmenu->x+wpopmenu->width, y - wpopmenu->height * ( i - wpopmenu->selected));
	      glEnd();
	    }
	  glBegin(GL_LINES);
	  glVertex2d(x, y-wpopmenu->height*(i-1-wpopmenu->selected) );
	  glVertex2d(x, y-wpopmenu->height*(i-wpopmenu->selected));
	  glEnd();
	  glBegin(GL_LINES);
	  glVertex2d(wpopmenu->x + wpopmenu->width, y - wpopmenu->height * ( i - 1 - wpopmenu->selected) );
	  glVertex2d(wpopmenu->x + wpopmenu->width, y - wpopmenu->height * ( i - wpopmenu->selected));
	  glEnd();
	  menu = getMenu(wpopmenu, i);
	  if( menu != NULL )
	    {
	      if( i == wpopmenu->newselect )
		glColor3f(.1, 1.0, .1);
	      drawText(gameFtx, x+10, y-wpopmenu->height*(i-wpopmenu->selected)+4, s, menu->name);
	      glColor3f(1.0, 1.0, 1.0);
	    }
	}
    }
}

static int
find_line(Wpopmenu *wpopmenu, Wpoint mousexy)
{
  int line = -1;
  int tmp;

  if( mousexy.h < wpopmenu->x+(strlen(wpopmenu->title)+2)*(wpopmenu->height-8) || mousexy.h > wpopmenu->x+wpopmenu->width )
    return -1;

  tmp  = mousexy.v - wpopmenu->y;
  if( tmp < 0 )
    tmp = tmp/wpopmenu->height-1;
  else
    tmp = tmp/wpopmenu->height;
  line = wpopmenu->selected - tmp;

  printf("line is %d\n", line);
  return line;
}

void
mouse_wpopmenu(Wpopmenu *wpopmenu, int buttons, int state, int dblClick, Wpoint mousexy)
{
  int line;

  //Check where in the popmenu to change selection.
  if((buttons == 1) && (state == SDL_PRESSED))
    {
      wpopmenu->isOpen = 1;
      printf("open menu pop up %s pressed\n", wpopmenu->title);
      //wpopmenu->newselect=wpopmenu->selected;

      line = find_line(wpopmenu, mousexy);
      if( line < 0 || line >= wpopmenu->nbMenus )
       {
	 printf("close menu pop up%s pressed\n", wpopmenu->title);
	 wpopmenu->isOpen = 0;
	 if( wpopmenu->action != NULL )
	   wpopmenu->action(wpopmenu);
       } else
	 wpopmenu->newselect=line;

    } else {
      wpopmenu->isOpen = 0;
      if( wpopmenu->action != NULL )
	wpopmenu->action(wpopmenu);
      printf("close menu pop up %s unpressed\n", wpopmenu->title);
      wpopmenu->selected=wpopmenu->newselect;

    }
}


void
mouseMotion_wpopmenu(Wpopmenu *wpopmenu, Wpoint mousexy )
{
 int line;

 if( wpopmenu->isOpen == 1 )
   {
     line = find_line(wpopmenu, mousexy);
     printf("+ mouseMotion_wpopmenu : line is %d\n", line);
     if( line < 0 || line >= wpopmenu->nbMenus )
       {
	 wpopmenu->isOpen = 0;
	 if( wpopmenu->action != NULL )
	   wpopmenu->action(wpopmenu);
	 wpopmenu->selected=wpopmenu->newselect;
       } else {
	 wpopmenu->newselect=line;
	 printf("new selection is %d\n", wpopmenu->newselect);
       }
   }
}

Wrect
getRect_wpopmenu( Wpopmenu *wpopmenu )
{
  Wrect rect;

  if( wpopmenu->isOpen != 1 )
    {
      rect.top    = wpopmenu->y +wpopmenu->height;
      rect.bottom = wpopmenu->y ;
      rect.left   = wpopmenu->x;
      rect.right  = wpopmenu->x + wpopmenu->width;
    } else {
      rect.top    = wpopmenu->y+wpopmenu->height*(wpopmenu->selected+1);
      rect.left   = wpopmenu->x+(strlen(wpopmenu->title)+2)*(wpopmenu->height-8);
      rect.right  = wpopmenu->x + wpopmenu->width;
      rect.bottom = wpopmenu->y-wpopmenu->height*(wpopmenu->nbMenus-wpopmenu->selected);
    }

  return rect;
}

