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


Wtext   *new_wtext(int width, int height, int posx, int posy, int nblines);
void     scroll_wtext(Wtext *wtext, int size);
void     insert_wtext(Wtext *wtext, char *text, int color_code);
void     draw_wtext(Wtext *wtext);
void     free_wtext(Wtext *wtext);
#endif
