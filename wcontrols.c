#include <stdlib.h>
#include <SDL.h>
#include "wcontrols.h"

WrootControl*
newRootControl()
{
  WrootControl   *root;

  root = (WrootControl   * ) malloc(sizeof( WrootControl ) );

  root->controlList       = NULL;
  root->currentcontrol    = 0;
  root->lastClick         = 0;
  return root;
}

void
newControl( WrootControl  *root, Wptr control, int type )
{
  WcontrolRef wcontrol, tmp;

  if( control == NULL )
    return;

  wcontrol = ( WcontrolRef ) malloc(sizeof(Wcontrol));

  wcontrol->next = NULL;
  wcontrol->visible = 1;
  wcontrol->enable = 1;
  wcontrol->highlight=0;
  wcontrol->type = type;
  wcontrol->control = control;
  
  //setting the rect
  switch(wcontrol->type )
    {
    case WcontrolButton:
      break;
    case WprogressBar:
/*       wcontrol->controlRect.top = (Wprogressbar*)(wcontrol->control)->x; */
/*       wcontrol->controlRect.left = (Wprogressbar*)(wcontrol->control)->y; */
/*       wcontrol->controlRect.bottom = (Wprogressbar*)(wcontrol->control)->x+(Wprogressbar*)(wcontrol->control)->height; */
/*      (wcontrol->controlRect.right = (Wprogressbar*)(wcontrol->control)->y+(Wprogressbar*)(wcontrol->control)->width; */
      break;
    case WprogressStatus:
/*       wcontrol->controlRect.top = (Wprogressstatus*)(wcontrol->control)->x; */
/*       wcontrol->controlRect.left = (Wprogressstatus*)(wcontrol->control)->y; */
/*       wcontrol->controlRect.bottom = (Wprogressstatus*)(wcontrol->control)->x+(Wprogressstatus*)(wcontrol->control)->height; */
/*       wcontrol->controlRect.right = (Wprogressstatus*)(wcontrol->control)->y+(Wprogressstatus*)(wcontrol->control)->width; */
      break;
    case WinputText:
/*       wcontrol->controlRect.top = (Wintext*)(wcontrol->control)->x; */
/*       wcontrol->controlRect.left = (Wintext*)(wcontrol->control)->y; */
/*       wcontrol->controlRect.bottom = (Wintext*)(wcontrol->control)->x+(Wintext*)(wcontrol->control)->height; */
/*       wcontrol->controlRect.right = (Wintext*)(wcontrol->control)->y+(Wintext*)(wcontrol->control)->width; */
      break;
    case WoutputText:
/*       wcontrol->controlRect.top = (Wtext*)(wcontrol->control)->x; */
/*       wcontrol->controlRect.left = (Wtext*)(wcontrol->control)->y; */
/*       wcontrol->controlRect.bottom = (Wtext*)(wcontrol->control)->x+(Wtext*)(wcontrol->control)->height; */
/*       wcontrol->controlRect.right = (Wtext*)(wcontrol->control)->y+(Wtext*)(wcontrol->control)->width; */
      break;
    case  WstaticText:
      break;
    case Wlistbox:
      wcontrol->controlRect = getRect_wlist((Wlist*)wcontrol->control);
/*       wcontrol->controlRect.top = (Wlist*)(wcontrol->control)->x; */
/*       wcontrol->controlRect.left = (Wlist*)(wcontrol->control)->y; */
/*       wcontrol->controlRect.bottom = (Wlist*)(wcontrol->control)->x+(Wlist*)(wcontrol->control)->height; */
/*       wcontrol->controlRect.right = (Wlist*)(wcontrol->control)->y+(Wlist*)(wcontrol->control)->width;; */
      break;
    case Wscrollbar:	  
      break;
    }


  //adding to the list in queue

  tmp = root->controlList;

  if( tmp == NULL )
    {
      root->controlList = wcontrol;
    } else {
      while( tmp->next != NULL ) { tmp = tmp->next; }
      tmp->next = wcontrol;
    }
}

WcontrolRef
getCurrentControl( WrootControl  *root )
{
  WcontrolRef wcontrol;
  int         i;

  if( root->currentcontrol == 0 )
    {
      return NULL;
    } else {
      i = 0;
      wcontrol = root->controlList;

      while( ( ++i != root->currentcontrol) && (wcontrol != NULL) )
	{
	  wcontrol  = wcontrol->next;
	}
      if( i != root->currentcontrol )
	return NULL;
      else
	return wcontrol;
    }
}


void
setCurrentControl( WrootControl  *root, Wptr controldata )
{
   WcontrolRef wcontrol;
   int i = 1;
 
   wcontrol = root->controlList;

   while( ( wcontrol != NULL ) && ( wcontrol->control != controldata ) )
     {
       i++;
       wcontrol = wcontrol->next;
     }
   if( wcontrol == NULL )
     return;
   root->currentcontrol = i;
}

void
updateControls( WrootControl  *root )
{
  WcontrolRef wcontrol;  


  wcontrol = root->controlList;

  /** here we draw all controls */
  while (wcontrol != NULL)
    {
      switch( wcontrol->type )
	{
	case WcontrolButton:
	  break;
	case WprogressBar:
	  draw_wprogressbar((Wprogressbar*)wcontrol->control);
	  break;
	case WprogressStatus:
	  draw_wprogressstatus((Wprogressstatus*)wcontrol->control);
	  break;
	case WinputText:
	  draw_wintext((Wintext*)wcontrol->control);
	  break;
	case WoutputText:
	  draw_wtext((Wtext*)wcontrol->control);
	  break;
	case  WstaticText:
	  break;
	case Wlistbox:
	  draw_wlist((Wlist *)wcontrol->control);
	  break;
	case Wscrollbar:	  
	  break;
	}

      wcontrol = wcontrol->next;
    }
  
  //drawing the highlighting ( TODO )
}

static int
pointinRect( Wpoint pt, Wrect rect )
{
  printf("checking %d >= %d\n",  pt.v, rect.top);
  printf("checking %d <= %d\n",  pt.v, rect.bottom);
  printf("checking %d <= %d\n",  pt.h, rect.right);
  printf("checking %d >= %d\n",  pt.h, rect.left);
  return(    pt.v >= rect.top && pt.v <= rect.bottom
	     && pt.h <= rect.right && pt.h >= rect.left );
}

void
clickControls( WrootControl  *root, int buttons, int state, Wpoint mousexy )
{
  WcontrolRef wcontrol;
  int         dblClick = 0;

  globalMouseToLocal(&mousexy);

  //first we need to find on which control do we click
  wcontrol = root->controlList;

  if( state == SDL_PRESSED )
    {
      printf(" check if double click %d %d\n", root->lastClick, SDL_GetTicks());
      dblClick = (SDL_GetTicks() - root->lastClick  )<= 250;
      root->lastClick =  SDL_GetTicks();
    }

  while( wcontrol != NULL &&
	 ! pointinRect(mousexy, wcontrol->controlRect ) )
    {
      wcontrol = wcontrol->next;
    }

  if( wcontrol == NULL )
    return;

  switch( wcontrol->type )
    {
    case WcontrolButton:
      break;
    case WprogressBar:
      break;
    case WprogressStatus:
      break;
    case WinputText:
      break;
    case WoutputText:
      break;
    case  WstaticText:
      break;
    case Wlistbox:
      mouse_wlist((Wlist *)wcontrol->control, buttons, state, dblClick, mousexy);
      break;
    case Wscrollbar:	  
      break;
    }

}


void
mouseControls( WrootControl  *root, Wpoint mousexy )
{

}


void
keyControls(WrootControl  *root, int key )
{
  WcontrolRef wcontrol;

  if( root->currentcontrol == 0 )
    return;

  wcontrol = getCurrentControl(root);

  switch( wcontrol->type )
    {
    case WcontrolButton:
      break;
    case WprogressBar:
      break;
    case WprogressStatus:
      break;
    case WinputText:
      break;
    case WoutputText:
      break;
    case  WstaticText:
      break;
    case Wlistbox:
      key_wlist((Wlist *)wcontrol->control, key);
      break;
    case Wscrollbar:	  
      break;
    }
}

void
globalMouseToLocal(Wpoint *mousexy)
{
  SDL_Surface *screen = SDL_GetVideoSurface();

  mousexy->v=screen->h-mousexy->v;
}


void
freeRootControl( WrootControl *root)
{
  //free all controls
  WcontrolRef wcontrol;  


  wcontrol = root->controlList;

  /** here we draw all controls */
  while (wcontrol != NULL)
    {
      switch( wcontrol->type )
	{
	case WcontrolButton:
	  break;
	case WprogressBar:
	  free_wprogressbar((Wprogressbar*)wcontrol->control);
	  break;
	case WprogressStatus:
	  free_wprogressstatus((Wprogressstatus*)wcontrol->control);
	  break;
	case WinputText:
	  free_wintext((Wintext*)wcontrol->control);
	  break;
	case WoutputText:
	  free_wtext((Wtext*)wcontrol->control);
	  break;
	case  WstaticText:
	  break;
	case Wlistbox:
	  free_wlist((Wlist *)wcontrol->control);
	  break;
	case Wscrollbar:	  
	  break;
	}
      wcontrol = wcontrol->next;
    }

  //Free RootControl
  free(root);

}
