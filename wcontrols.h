/** 
 *          File       :  wcontrols.h
 *
 *          Contains   :  controls widget, management of controls.
 *
 *          Version    :  1.0 ( 02/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */

#ifndef __WCONTROLS_H__
#define __WCONTROLS_H__

#ifndef __WTYPES_H__
#include "wtypes.h"
#endif

#ifndef __WTEXT_H__
#include "wtext.h"
#endif

#ifndef __WSTATICTEXT_H__
#include "wstatictext.h"
#endif

#ifndef __WPROGRESS_H__
#include "wprogress.h"
#endif

#ifndef __WBUTTON_H__
#include "wbutton.h"
#endif

#ifndef __WPOPMENU_H__
#include "wpopmenu.h"
#endif

#ifndef __WCHECKBOX_H__
#include "wcheckbox.h"
#endif

#ifndef __WLIST_H__
#include "wlist.h"
#endif

/** Controls */
typedef struct ControlRecord Wcontrol;
typedef Wcontrol* WcontrolRef;

/*
 *   Control record
 */
struct ControlRecord
{
  WcontrolRef   next;            //next control
  WcontrolRef   prev;            //previous control
  Wrect         controlRect;     //position of the control and size.
  int           visible;         //if control is visible
  int           enable;          //if control is enable
  int           highlight;       //if control is highlighted
  int           type;            //type of control
  Wptr          control;         //pointer to the control.
};

/*
 *   Root Control
 */
typedef struct WrootControl
{
  WcontrolRef     controlList;   //control list
  WcontrolRef     drawList;      //control list
  int             currentcontrol;//current control selected id
  int             lastClick;     //time of last click ( for double clicking )
  int             currentFocus;  //current controls focused
  int             lastFocus;     //time of the last focuse
  int             activeFocus;   //if focus is activated.
} WrootControl;

/** definitions of controls */

/*
 *  Buttons
 */
enum {
  WcontrolButton  =  10         //a simple default button 
};

/*
 *  Progress indicators
 */
enum {
  WprogressBar    =  20,        //a progressbar
  WprogressStatus =  21         //a progressstatus
};

/*
 *  Edit text
 */
enum {
  WinputText     =  30,         //an input entry textbox
  WoutputText    =  31          //an output not editable textbox
};

/*
 *  Static text
 */
enum {
  WstaticText    =  40          //a static text box ( not yet implemented )
};

/*
 *  List box
 */
enum {
  Wlistbox       =  50          //a list box
};

/*
 *  Scrollbar
 */
enum {
  Wscrollbar     =  60          //a scroll bar ( not yet implemented )
};

/*
 *  PopupMenu
 */
enum {
  WpopupMenu     = 70           //a menu pop up
};


/** Creating Root Control               */
WrootControl*     newRootControl();

/** Creating a Control                  */
void              newControl( WrootControl  *root, Wptr control, int type );

/** Get current control                 */
WcontrolRef       getCurrentControl( WrootControl  *root );
WcontrolRef       getControl( WrootControl  *root, int controlID );

/** Updates controls                    */
void              updateControls( WrootControl  *root );

/** Click Event                         */
void              clickControls( WrootControl  *root, int buttons, int state, Wpoint mousexy );

/** Mouse Event                         */
void              mouseControls( WrootControl  *root, Wpoint mousexy );

/** Key Event                           */
void              keyControls(WrootControl  *root, int key, int unicode );

/** Set current control                 */
void              setCurrentControl( WrootControl  *root, Wptr controldata );

/** Free Root Control and its controls  */
void              freeRootControl( WrootControl *root);

/** Update Rects */
void              updateRectsControls( WrootControl *root );

/** Active or Deactive controls */
void              activateControl(   WrootControl *root, Wptr controldata );
void              deActivateControl( WrootControl *root, Wptr controldata );

void globalMouseToLocal(Wpoint *mousexy);
#endif
