#include <stdlib.h>
#include <SDL.h>
#include "mouse.h"
#include "wcontrols.h"

WrootControl*
newRootControl()
{
  WrootControl   *root;

  root = (WrootControl   * ) malloc(sizeof( WrootControl ) );

  root->controlList       = NULL;
  root->currentcontrol    = 0;
  root->lastClick         = 0;
  root->currentFocus      = -1;
  root->lastFocus         = 0;
  root->activeFocus       = 0;
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
      wcontrol->controlRect = getRect_wbutton((Wbutton*)wcontrol->control);
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
      wcontrol->controlRect = getRect_wstatictext((Wstatictext*)wcontrol->control);
      break;
    case Wlistbox:
      wcontrol->controlRect = getRect_wlist((Wlist*)wcontrol->control);
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


WcontrolRef
getControl( WrootControl  *root, int controlID )
{
  WcontrolRef wcontrol;
  int         i;

  if( root->currentcontrol == 0 )
    {
      return NULL;
    } else {
      i = 0;
      wcontrol = root->controlList;

      while( ( ++i != controlID) && (wcontrol != NULL) )
	{
	  wcontrol  = wcontrol->next;
	}
      if( i != controlID )
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
  int nx, ny;
  Wpoint pt;

  wcontrol = root->controlList;

  /** here we draw all controls */
  while (wcontrol != NULL)
    {
      switch( wcontrol->type )
	{
	case WcontrolButton:
	  draw_wbutton((Wbutton*)wcontrol->control);
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
	  draw_wstatictext((Wstatictext*)wcontrol->control);
	  break;
	case Wlistbox:
	  draw_wlist((Wlist *)wcontrol->control);
	  break;
	case Wscrollbar:	  
	  break;
	}

      wcontrol = wcontrol->next;
    }
  
  //drawing mouse focus
  //printf("SDL_GetTicks() %d root->lastFocus %d\n", SDL_GetTicks(), root->lastFocus);
  if( root->activeFocus && (SDL_GetTicks() - root->lastFocus) > 1500 )
    {
      wcontrol = getControl( root, root->currentFocus );

      getMouse( &nx, &ny );
      pt.v=ny;
      pt.h=nx;

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
	  root->activeFocus = mouseFocus_wlist((Wlist *)wcontrol->control, pt );
	  break;
	case Wscrollbar:	  
	  break;
	}
    }

  //drawing the highlighting ( TODO )
}

static int
pointinRect( Wpoint pt, Wrect rect )
{
  /* printf("checking %d >= %d\n",  pt.v, rect.top); */
/*   printf("checking %d <= %d\n",  pt.v, rect.bottom); */
/*   printf("checking %d <= %d\n",  pt.h, rect.right); */
/*   printf("checking %d >= %d\n",  pt.h, rect.left); */
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
    case WstaticText:
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
  WcontrolRef wcontrol;
  int         i = 0;

  globalMouseToLocal(&mousexy);

  //managing mouse focus
  wcontrol = root->controlList;
  if( wcontrol != NULL )
    i++;

  while( wcontrol != NULL &&
	 ! pointinRect(mousexy, wcontrol->controlRect ) )
    {
      wcontrol = wcontrol->next;
      i++;
    }
  
  if( wcontrol == NULL )
    {
      root->activeFocus  = 0;
      root->currentFocus = -1;
      return;
    }
 


  if( root->activeFocus == 1 && i == root->currentFocus )
    return;
    
  root->currentFocus  = i;
  root->activeFocus   = 1;
  root->lastFocus     = SDL_GetTicks();

  printf("new focus is %d\n", root->currentFocus );

/*   switch( wcontrol->type ) */
/*     { */
/*     case WcontrolButton: */
/*       break; */
/*     case WprogressBar: */
/*       break; */
/*     case WprogressStatus: */
/*       break; */
/*     case WinputText: */
/*       break; */
/*     case WoutputText: */
/*       break; */
/*     case  WstaticText: */
/*       break; */
/*     case Wlistbox: */
/*       mouseMotion_wlist((Wlist *)wcontrol->control, mousexy); */
/*       break; */
/*     case Wscrollbar:	   */
/*       break; */
/*     } */

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
	  free_wbutton((Wbutton*)wcontrol->control);
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
	case WstaticText:
	  free_wstatictext((Wstatictext*)wcontrol->control);
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
