#ifndef __WTEXT_H__
#define __WTEXT_H__

typedef struct {
  int              width;             //width of the text in line
  int              height;            //height of the text in chars
  int              x;                 //pos of top left corner
  int              y;
  int              current;           //is current line
  char**           buffer;            //text buffer
} Wtext;


Wtext   *new_wtext(int width, int height, int posx, int posy);
void     scroll_wtext(Wtext *wtext, int size);
void     insert_wtext(Wtext *wtext, char *text, int color_code);
void     draw_wtext(Wtext *wtext);
void     free_wtext(Wtext *wtext);
#endif
