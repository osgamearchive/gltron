/** 
 *          File       :  wpopmenu.h
 *
 *          Contains   :  A popup menu widget.
 *
 *          Version    :  1.0 ( 06/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */

#ifndef __WPOPMENU_H__
#define __WPOPMENU_H__

/** Definition of a menu from a pop up menu */
typedef struct MenuRecord Wmenu;

struct MenuRecord {
  Wmenu           *next;              //next menu
  char             name[255];         //name printed
  int              value;             //value of the selection.
}; 

/** Definistion of a pop up menu  */
typedef struct PopmenuRecord Wpopmenu;
struct PopmenuRecord {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  char             title[255];        //title of the menu can be ""
  int              selected;          //menu selected selected
  int              newselect;         //oldselection
  int              isOpen;            //if the menu is open
  int              nbMenus;           //nb menus
  void           (*action)(Wpopmenu *wpopmenu); //action 
  Wmenu           *menuList;          //list of menu
};

/** Creating a new pop up menu */
Wpopmenu*    new_wpopmenu(int x, int y, int width, int height, char *title,
			 void           (*action)(Wpopmenu *wpopmenu) );

/** Adding a menu choice */
void         addchoice_wpopmenu(Wpopmenu *wpopmenu, char *name, int value);

/** Selection of a choice */
void         select_wpopmenu(Wpopmenu *wpopmenu, int menuchoice );

/** Getting current val */
int          getValue_wpopmenu(Wpopmenu *wpopmenu);

/** Set title */
void         setTitle_wpopmenu(Wpopmenu *wpopmenu, char *title);

/** Destroy the popmenu */
void         free_wpopmenu(Wpopmenu *wpopmenu);

/** Draw the popmenu */
void         draw_wpopmenu(Wpopmenu *wpopmenu, int active);

/** Mouse Events */
void         mouse_wpopmenu(Wpopmenu *wpopmenu, int buttons, int state, int dblClick, Wpoint mousexy);
void         mouseMotion_wpopmenu(Wpopmenu *wpopmenu, Wpoint mousexy );

/**Get Rect */
Wrect        getRect_wpopmenu(Wpopmenu *wpopmenu);

#endif
