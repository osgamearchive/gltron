/** 
 *          File       :  wstatictext.h
 *
 *          Contains   :  Static Text control.
 *
 *          Version    :  1.0 ( 03/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */
#ifndef __WSTATICTEXT_H__
#define __WSTATICTEXT_H__

/** Fonts type */
enum {
  guiFont,
  gameFont,
  netFont
};

/** Definistion of a static text  */
typedef struct Wstatictext {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  int              size;              //size of text
  int              font;              //font of text
  float            color[3];          //color
  char            *text;              //data of the text.
} Wstatictext;

/** Creation of a static text */
Wstatictext *new_wstatictext( int x, int y, int width, int height, char *text,
			      int size, int font, float color[3]);

/** Draw a static text */
void         draw_wstatictext(Wstatictext *wstatictext);

/** Change a static text */
void         set_wstatictext(Wstatictext *wstatictext, char *ntext);

/** Distroy a static text */
void         free_wstatictext(Wstatictext *wstatictext);

/** Get Rect */
Wrect        getRect_wstatictext( Wstatictext *wstatictext );
#endif

