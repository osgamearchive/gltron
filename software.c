#include "gltron.h"

typedef struct {
  int size;
  int lower;
  int upper;
  char **glyphs;
} bitmapFont;

static list *bitmapFonts;

void initBitmapFonts() { /* must be called before usage of bitmap fonts */
  bitmapFonts = (list*) malloc(sizeof(list));
  bitmapFonts->next = NULL;
}

bitmapFont* makeBitmapFont(fonttex *tex, int size) {
#define CHARWIDTH 16
  
  return 0;
}

bitmapFont* getBitmapFont(fonttex *tex, int size) {
  list *p;

  for(p = bitmapFonts; p->next != NULL; p = p->next) {
    if( ((bitmapFont*) p->data)->size == size)
      return (bitmapFont*) p->data;
  }
  p->next = (list*) malloc(sizeof(list));
  p->next->next = NULL;
  p->data = makeBitmapFont(tex, size);
  return (bitmapFont*) p->data;
}
  
void drawSoftwareText(fonttex *tex, int x, int y, int size, char *text) {
  bitmapFont *font;
  int pos = 0;
  char c;

  font = getBitmapFont(tex, size);

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
