#ifndef __WTEXT_H__
#define __WTEXT_H__

typedef struct {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  int              nblines;           //number of lines
  int              nbchars;           //number of chars by line
  int              current;           //is current line
  int              cur_char;          //is current char in current line.
  char**           buffer;            //text buffer
} Wtext;

typedef struct CHistory *History;

typedef struct CHistory {
  char             *text;            //text
  History           next;            //next text
} CHistory;

typedef struct {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  int              nbchars;           //number of chars on the line
  int              cur_char;          //is current char in current line.
  int              maxchars;          //max chars that can be red
  int              time;              //time
  int              offset;
  int              wstart;            //used to scroll
  int              wend;              
  History          history;
  int              h_num;             //history number
  char*            buffer;            //text buffer  
} Wintext;

//Wtext functions : printing text.
Wtext   *new_wtext(int width, int height, int posx, int posy, int nblines);
void     scroll_wtext(Wtext *wtext, int size);
void     insert_wtext(Wtext *wtext, char *text, int color_code);
void     draw_wtext(Wtext *wtext);
void     free_wtext(Wtext *wtext);

//Wintext functions : read and print input text.
Wintext *new_wintext(int width, int height, int posx, int posy, int nbchars, int maxchars);
void     free_wintext(Wintext *wintext);
void     key_wintext(Wintext  *wintext, int charcode, int unicode);
void     draw_wintext(Wintext *wintext);
char    *get_wintext(Wintext  *wintext);
void     clear_wintext(Wintext *wintext);
#endif
