#include "gltron.h"

typedef struct {
  int size;
  int lower;
  int upper;
  int channels;
  unsigned char **glyphs;
} bitmapFont;

static list *bitmapFonts = NULL;

void deleteBitmaps(gDisplay *d) {
  /* keep bitmapFonts around */
  fbmpUnloadFont(d->bitfont);
  free(d->pixelGui);
}

void initBitmaps(gDisplay *d) { 
  /* must be called before usage of bitmap stuff */
  if(bitmapFonts == NULL) {
      bitmapFonts = (list*) malloc(sizeof(list));
      bitmapFonts->next = NULL;
  }
  d->bitfont = fbmpLoadFont("test.fbmp");
  d->pixelGui = loadPixels("gltron_bitmap.png", d);
}

bitmapFont* makeBitmapFont(fontbmp *fbmp, int size) {
  bitmapFont *font;
  int i;

  font = (bitmapFont*) malloc(sizeof(bitmapFont));
  font->lower = fbmp->lower;
  font->upper = fbmp->upper;
  font->size = size;
  font->channels = fbmp->tex->channels;
  font->glyphs = (unsigned char**) malloc( (font->upper - font->lower) * (sizeof(unsigned char*)) );
  for(i = 0; i < font->upper - font->lower; i++) {
    int x, y;
    x = (i % ( fbmp->texwidth / fbmp->width )) * fbmp->width;
    y = fbmp->texwidth - fbmp->width - 
      (i / ( fbmp->texwidth / fbmp->width )) * fbmp->width;
#ifndef BITMAP
    font->glyphs[i] = scalePixels(fbmp->tex->data,
				  fbmp->texwidth, fbmp->texwidth,
				  x, y, fbmp->width, fbmp->width,
				  size, size, font->channels);
#endif
#ifdef BITMAP
    font->glyphs[i] = scalePixelBitmap(fbmp->tex->data,
				  fbmp->texwidth, fbmp->texwidth,
				  x, y, fbmp->width, fbmp->width,
				  size, size, font->channels);
#endif
  }
  return font;
}

bitmapFont* getBitmapFont(int size) {
  list *p;

  for(p = bitmapFonts; p->next != NULL; p = p->next) {
    if( ((bitmapFont*) p->data)->size == size)
      return (bitmapFont*) p->data;
  }
  p->next = (list*) malloc(sizeof(list));
  p->next->next = NULL;
  printf("making new bitmapfont of size %d\n", size);
  p->data = makeBitmapFont(game->screen->bitfont, size);
  return (bitmapFont*) p->data;
}

void drawSoftwareHighlight(int x, int y, int size, char *text) {
#define PADDING 5
  glBegin(GL_LINE_LOOP);
  glVertex2i(x - PADDING, y - PADDING);
  glVertex2i(x + PADDING + strlen(text) * size, y - PADDING);
  glVertex2i(x + PADDING + strlen(text) * size, y + PADDING + size);
  glVertex2i(x - PADDING, y + PADDING + size);
  glEnd();
}

void drawSoftwareText(fonttex *tex, int x, int y, int size, char *text) {
  bitmapFont *font;
  int pos = 0;
  char c;

  font = getBitmapFont(size);
  if(font == NULL) {
    fprintf(stderr, "got null font, size %d\n", size);
  } else {
    while(*(text + pos) != 0) {
      glRasterPos2i(x + pos * size, y);
      c = *(text + pos);
      if(c > font->lower && c < font->upper) {
	int type;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	type = (font->channels == 4) ? GL_RGBA : GL_RGB;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifndef BITMAP
	glDrawPixels(font->size, font->size, type, GL_UNSIGNED_BYTE,
		     font->glyphs[c - font->lower + 1]);
#endif
#ifdef BITMAP
	glBitmap(font->size, font->size, 0, 0, font->size, 0,
		 font->glyphs[c - font->lower + 1]);
#endif
	glDisable(GL_BLEND);
      }
      pos++;
    }
  }
}
