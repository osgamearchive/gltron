/** 
 *          File       :  wcheckbox.h
 *
 *          Contains   :  A check box widget.
 *
 *          Version    :  1.0 ( 08/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */

#ifndef __WCHECKBOX_H__
#define __WCHECKBOX_H__
/** The button definition  */

typedef struct CheckboxRecord Wcheckbox;

struct CheckboxRecord {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  char             title[255];        //title of the checkbox.
  int              checked;           //if checked
  void           (*action)( Wcheckbox *wcheckbox );
};

/** Creating the checkBox   */
Wcheckbox*  new_wcheckbox( int x, int y, int width, int height, char *title,
			   void (*action)( Wcheckbox *wcheckbox ));

/** Draw the checkbox       */
void        draw_wcheckbox( Wcheckbox *wcheckbox, int active );

/** Destroy the checkbox */
void        free_wcheckbox( Wcheckbox *wcheckbox );

/** Get Rect           */
Wrect       getRect_wcheckbox(Wcheckbox *wcheckbox );

/** Set checkbox */
void        set_wcheckbox(Wcheckbox *wcheckbox, int checked );

/** Get checkbox status */
int        get_wcheckbox(Wcheckbox *wcheckbox );

/** Mouse events        */
void        mouse_wcheckbox( Wcheckbox *wcheckbox, int buttons, int state, int dblClick, Wpoint mousexy );

#endif __WCHECKBOX_H__
