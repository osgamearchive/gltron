#ifndef __WLIST_H__
#define __WLIST_H__

enum {
  LIST_SCROLL_UP,
  LIST_SCROLL_DOWN
};

/** Columns data for a line */
typedef struct Columns *Columnsp;

typedef struct Columns {
  char     *data; //data
  Columnsp next;
} Columns;


/** Lines of the list */
typedef struct Lines *Linesp;

typedef struct Lines {
  Columns   *cols;
  Linesp     next;
} Lines;

/** The list itself  */
typedef struct {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  int              nblines;           //number of lines
  int              nbcols;            //number of chars by line
  char            **titles;           //titles of columns
  int              current;           //is current line
  int              scroll;            //window that shows nblines
  int              rlines;            //how many lines in memory?
  char          ***lines;             //data on lines
} Wlist;

Wlist *new_wlist(  int x, int y, int width, int height, int nblines, int nbcols, char *titles[] );
void   draw_wlist       ( Wlist *wlist, Trackerslots *slots);
void   newline_wlist    ( Wlist *wlist, char *cols[]);
void   updatecell_wlist ( Wlist *wlist, char *val, int l, int c);
void   scroll_wlist     ( Wlist *wlist, int dir );
char  *getcell_wlist    ( Wlist *wlist, int l, int c );
int    getcurrent_wlist ( Wlist *wlist );
void   free_wlist       ( Wlist *wlist );
#endif
