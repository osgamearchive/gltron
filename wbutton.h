/** 
 *          File       :  wbutton.h
 *
 *          Contains   :  button widget.
 *
 *          Version    :  1.0 ( 0/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */

#ifndef __WBUTTON_H__
#define __WBUTTON_H__

/** The button definition  */

typedef struct ButtonRecord Wbutton;
typedef Wbutton* WbuttonRef;

struct ButtonRecord {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  char             title[255];        //title of the button.
  void             (*drawit)(WbuttonRef wbutton); //draw the button
  void             (*action)(WbuttonRef wbutton); //when click or press enter
  void             (*focus) (WbuttonRef wbutton); //when focus on button
  int              focused;
  void             (*mouseFocus) (WbuttonRef wbutton); //when focus on button
};

/** Creating the Button               */
Wbutton*   new_wbutton( int x, int y, int width, int height,
			char *title,
			void (*drawit)(WbuttonRef wbutton),
			void (*action)(WbuttonRef wbutton),
			void (*focus) (WbuttonRef wbutton),
			void (*mouseFocus) (WbuttonRef wbutton));

/** Draw the button */
void         draw_wbutton(Wbutton *wbutton);

/** Destroy the button */
void         free_wbutton(Wbutton *wbutton);

/** Get Rect */
Wrect        getRect_wbutton( Wbutton *wbutton );

/** Mouse events */
void mouse_wbutton( Wbutton *wbutton, int buttons, int state, int dblClick, Wpoint mousexy );
void mouseMotion_wbutton( Wbutton *wbutton, Wpoint mousexy, int active );
int  mouseFocus_wbutton( Wbutton *wbutton, Wpoint mousexy );

#endif
