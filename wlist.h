#ifndef __WLIST_H__
#define __WLIST_H__

enum {
  LIST_SCROLL_UP,
  LIST_SCROLL_DOWN
};

typedef struct Wlist *WlistPtr;

/** Callbacks for each columns */
typedef struct ColCallBacks
{
  void  (*drawit)( WlistPtr list, int x, int y, int line, int col );
  char* (*tostr)(WlistPtr list, int line, int col);
  int   (*sortit)( WlistPtr list, int line, int next );
} ColCallBacks;


/** Definition of each columns */
typedef struct ColDef
{
  char            *title;             //title of column
  int             colsize;            //size of each columns.
  float           color[3];           //color of the text.
  ColCallBacks    callbacks;          //callBacks for each columns
} ColDef;

/** The list itself  */
typedef struct Wlist {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  int              nblines;           //number of lines
  int              nbcols;            //number of cols by line
  ColDef          *colDefs;           //Definition of each columns
  int              current;           //is current line
  int              scroll;            //window that shows nblines
  int              rlines;            //how many lines in memory?
  int              sortcol;           //column for sorting
  void  (*focus) ( WlistPtr list, int line ); //focus on a line
  int             *index;             //index of lines
  char          ***lines;             //data on lines
} Wlist;


/** Creating and disposing of column defintions */
ColDef *new_colDefs( int nbcols );


/** Setting column defintions */
void    set_colDef( ColDef *colDefs, int col, char *title, int colsize,
		    float color[3],   
		    void  (*drawit)( WlistPtr list, int x, int y, int line, int col ),
		    char* (*tostr)(WlistPtr list, int line, int col),
		    int   (*sortit)( WlistPtr list, int line, int next )); 

/** Creating and disposing of lists */
Wlist *new_wlist(  int x, int y, int width, int height, int nblines, int nbcols, ColDef *colDefs, int sortcol, void  (*focus) ( WlistPtr list, int line ) );
void   free_wlist       ( Wlist *wlist );


/** Drawing the list */
void   draw_wlist       ( Wlist *wlist );


/** Accessing and Manipulating datas */
void   setCell_wlist    ( Wlist *wlist, char *data, int len, int line, int col );
char*  getCell_wlist    ( Wlist *wlist, int line, int col);
//void   clearCell_wlist  ( Wlist *wlist, Wcell theCell);


/** Adding and Deleting Rows */
int   addRow_wlist     ( Wlist *wlist, short nbrows );
//void   delRow_wlist     ( Wlist *wlist, short nbrows, short num );

/** Index Building */
void initindex_wlist(Wlist *wlist);
void rebuildindex_wlist(Wlist *wlist);


/** Scrolling the list */
void   scroll_wlist     ( Wlist *wlist, int dir );

/** Selections */
int getcurrent_wlist( Wlist *wlist );
#endif
