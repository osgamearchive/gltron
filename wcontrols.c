#include <stdlib.h>
#include <SDL.h>
#include "mouse.h"
#include "wcontrols.h"


static void mouseFocusControls( WrootControl  *root, Wpoint mousexy, WcontrolRef wcontrol );

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
  root->drawList          = NULL;
  return root;
}

void
newControl( WrootControl  *root, Wptr control, int type )
{
  WcontrolRef wcontrol, tmp;

  if( control == NULL )
    return;

  wcontrol = ( WcontrolRef ) malloc(sizeof(Wcontrol));

  wcontrol->next       = NULL;
  wcontrol->prev       = NULL;
  wcontrol->visible    = 1;
  wcontrol->enable     = 1;
  wcontrol->highlight  = 0;
  wcontrol->type       = type;
  wcontrol->control    = control;
  
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
      wcontrol->controlRect = getRect_wintext((Wintext*)wcontrol->control);
      break;
    case WoutputText:
      wcontrol->controlRect = getRect_wtext((Wtext*)wcontrol->control);
      break;
    case  WstaticText:
      wcontrol->controlRect = getRect_wstatictext((Wstatictext*)wcontrol->control);
      break;
    case Wlistbox:
      wcontrol->controlRect = getRect_wlist((Wlist*)wcontrol->control);
      break;
    case Wscrollbar:	  
      break;
    case WpopupMenu:
      wcontrol->controlRect = getRect_wpopmenu((Wpopmenu*)wcontrol->control);
      break;
    }


  //adding to the list in queue

/*   tmp = root->controlList; */

/*   if( tmp == NULL ) */
/*     { */
/*       root->controlList = wcontrol; */
/*     } else { */
/*       while( tmp->next != NULL ) { tmp = tmp->next; } */
/*       tmp->next = wcontrol; */
/*     } */
  tmp = root->controlList;
  if( tmp == NULL )
    root->drawList = wcontrol;
  else
    tmp->prev = wcontrol;
  root->controlList = wcontrol;
  wcontrol->next = tmp;

}

void
updateRectsControls( WrootControl *root )
{
  WcontrolRef wcontrol;

  wcontrol = root->controlList;

  while( wcontrol != NULL )
    {
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
	  wcontrol->controlRect = getRect_wintext((Wintext*)wcontrol->control);
	  break;
	case WoutputText:
	  wcontrol->controlRect = getRect_wtext((Wtext*)wcontrol->control);
	  break;
	case  WstaticText:
	  wcontrol->controlRect = getRect_wstatictext((Wstatictext*)wcontrol->control);
	  break;
	case Wlistbox:
	  wcontrol->controlRect = getRect_wlist((Wlist*)wcontrol->control);
	  break;
	case Wscrollbar:	  
	  break;
	case WpopupMenu:
	  wcontrol->controlRect = getRect_wpopmenu((Wpopmenu*)wcontrol->control);
	  break;
	}
      wcontrol=wcontrol->next;
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
  WcontrolRef current;
  int nx, ny;
  Wpoint pt;
  int    done = 0;

  current = getCurrentControl( root );
  wcontrol = root->drawList;
  

  /** here we draw all controls */
  while (wcontrol != NULL && done != 2)
    {
      if(  wcontrol == current  )
	{
	  if( done == 0 && wcontrol->prev != NULL  )
	    {
	      wcontrol = wcontrol->prev;
	    } else {
	      done = 2;
	      switch( wcontrol->type )
		{
		case WcontrolButton:
		  draw_wbutton((Wbutton*)wcontrol->control, wcontrol->enable);
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
		case WpopupMenu:
		  draw_wpopmenu((Wpopmenu*)wcontrol->control, wcontrol->enable);
		  break;
		}
	    }
	} else {
	  switch( wcontrol->type )
	    {
	    case WcontrolButton:
	      draw_wbutton((Wbutton*)wcontrol->control, wcontrol->enable);
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
	    case WpopupMenu:
	      draw_wpopmenu((Wpopmenu*)wcontrol->control, wcontrol->enable);
	      break;
	    }
	  if( wcontrol->prev == NULL )
	    {
	      wcontrol = current;
	      done = 1;
	    }  else {
	      wcontrol = wcontrol->prev;
	    }
	}
    }
  
  //drawing mouse focus
  //printf("SDL_GetTicks() %d root->lastFocus %d\n", SDL_GetTicks(), root->lastFocus);
  if( root->activeFocus && (SDL_GetTicks() - root->lastFocus) > 1500 )
    {
      wcontrol = getControl( root, root->currentFocus );
      if( wcontrol == NULL || wcontrol->enable == 0 )
	return;

      getMouse( &nx, &ny );
      pt.v=ny;
      pt.h=nx;

      switch( wcontrol->type )
	{
	case WcontrolButton:
	  root->activeFocus = mouseFocus_wbutton((Wbutton *)wcontrol->control, pt );
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
	case WpopupMenu:
	  //draw_wpopmenu((Wpopmenu*)wcontrol->control);
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
  return(    pt.v <= rect.top && pt.v >= rect.bottom
	     && pt.h <= rect.right && pt.h >= rect.left );
}

void
clickControls( WrootControl  *root, int buttons, int state, Wpoint mousexy )
{
  WcontrolRef wcontrol, oldcontrol;
  int         dblClick  = 0;
  int         controlID = 0;

  globalMouseToLocal(&mousexy);

  
  updateRectsControls( root );
  //first we need to find on which control do we click
  wcontrol = root->controlList;

  if( wcontrol != NULL )
    controlID++;

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
      controlID++;
    }

  if( controlID != root->currentcontrol )    
    //mouseFocusControls( root, mousexy, getCurrentControl( root ) );
    {
      oldcontrol = getCurrentControl( root );
      if( oldcontrol != NULL )
	{
	  switch( oldcontrol->type )
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
	      break;
	    case Wscrollbar:	  
	      break;
	    case WpopupMenu:
	      mouse_wpopmenu((Wpopmenu*)oldcontrol->control, buttons, !SDL_PRESSED, dblClick, mousexy);
	      break;
	    }
	}
    }

  if( wcontrol == NULL || wcontrol->enable == 0 )
      return;

  root->currentcontrol =  controlID;
  printf("change currentcontrol to %d\n", controlID);
  switch( wcontrol->type )
    {
    case WcontrolButton:
      mouse_wbutton((Wbutton *)wcontrol->control, buttons, state, dblClick, mousexy);
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
    case WpopupMenu:
      mouse_wpopmenu((Wpopmenu*)wcontrol->control, buttons, state, dblClick, mousexy);
      break;
    }

}


static void
mouseFocusControls( WrootControl  *root, Wpoint mousexy, WcontrolRef wcontrol )
{

  
  updateRectsControls( root );

  if( wcontrol == NULL || wcontrol->enable == 0 )
    return;

  switch( wcontrol->type )
    {
    case WcontrolButton:
      mouseMotion_wbutton((Wbutton *)wcontrol->control, mousexy, root->activeFocus);
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
      //mouseMotion_wlist((Wlist *)wcontrol->control, mousexy);
      break;
    case Wscrollbar:	  
      break;
    case WpopupMenu:
      mouseMotion_wpopmenu((Wpopmenu *)wcontrol->control, mousexy );
      break;
    }
}
void
mouseControls( WrootControl  *root, Wpoint mousexy )
{
  WcontrolRef wcontrol;
  int         i = 0;

  globalMouseToLocal(&mousexy);


  
  updateRectsControls( root );

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

      //find all wcontrol focused to deactivate
      wcontrol = root->controlList;
      i= 1;
      while( wcontrol != NULL && i++ < root->currentFocus )
	{
	  wcontrol = wcontrol->next;
	}
      if( wcontrol == NULL )
	return;
      
      mouseFocusControls( root, mousexy, wcontrol );
      root->currentFocus = -1;
    } else {
 
      if(  wcontrol->enable == 0 )
	return;

      /* if( root->activeFocus == 1 && i == root->currentFocus ) */
/* 	{ */
/* 	  return; */
/* 	} */
      
      if( root->activeFocus == 1 && i != root->currentFocus )
	{
	  //deactivate old control
	  mouseFocusControls( root, mousexy, getCurrentControl( root ) );	  
	}
      root->currentFocus  = i;
      root->activeFocus   = 1;
      root->lastFocus     = SDL_GetTicks();

      mouseFocusControls( root, mousexy, wcontrol );
      //printf("new focus is %d\n", root->currentFocus );
    }
}


void
keyControls(WrootControl  *root, int key, int unicode )
{
  WcontrolRef wcontrol;

  if( root->currentcontrol == 0 )
    return;

  wcontrol = getCurrentControl(root);
  if( wcontrol == NULL || wcontrol->enable == 0 )
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
      key_wintext((Wintext *)wcontrol->control, key, unicode);
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
    case WpopupMenu:
      //draw_wpopmenu((Wpopmenu*)wcontrol->control);
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
	case WpopupMenu:
	  free_wpopmenu((Wpopmenu*)wcontrol->control);
	  break;
	}
      wcontrol = wcontrol->next;
    }

  //Free RootControl
  free(root);

}


void
activateControl(   WrootControl *root, Wptr controldata )
{
  WcontrolRef       wcontrol;

  wcontrol = root->controlList;

  while( wcontrol != NULL && wcontrol->control != controldata )
    {
      wcontrol=wcontrol->next;
    }
  if( wcontrol == NULL )
    return;
  wcontrol->enable=1;
}

void
deActivateControl( WrootControl *root, Wptr controldata )
{
  WcontrolRef       wcontrol;

  wcontrol = root->controlList;

  while( wcontrol != NULL && wcontrol->control != controldata )
    {
      wcontrol=wcontrol->next;
    }
  if( wcontrol == NULL )
    return;
  wcontrol->enable=0;

}
