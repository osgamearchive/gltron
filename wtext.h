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
void     key_wintext(Wintext *wintext, int unicode);
void     draw_wintext(Wintext *wintext);
#endif
