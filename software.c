#include "gltron.h"

typedef struct {
  int size;
  int lower;
  int upper;
  char **glyphs;
} bitmapFont;

static texture *bitmapTex;
static list *bitmapFonts;

void initBitmapFonts() { /* must be called before usage of bitmap fonts */
  /*
  bitmapTex = loadTextureData( imcomplete );
  */
}
bitmapFont* makeBitmapFont(int size) {
  /*
  tex = loadTextureData(
  */
  return 0;
}

bitmapFont* getBitmapFont(int size) {
  list *p;

  for(p = bitmapFonts; p->next != NULL; p = p->next) {
    if( ((bitmapFont*) p->data)->size == size)
      return (bitmapFont*) p->data;
  }
  p->next = (list*) malloc(sizeof(list));
  p->next->next = NULL;
  p->data = makeBitmapFont(size);
  return (bitmapFont*) p->data;
}
  
void drawSoftwareText(int x, int y, int size, char *text) {
  bitmapFont *font;
  int pos = 0;
  char c;

  font = getBitmapFont(size);

  while(*(text + pos) != 0) {
    glRasterPos2i(x + pos * size, y);
    c = *(text + pos);
    if(c > font->lower && c < font->upper) {
      glDrawPixels(font->size, font->size, GL_RGBA, GL_UNSIGNED_BYTE,
		   font->glyphs[c - font->lower]);
    }
    pos++;
  }
}






