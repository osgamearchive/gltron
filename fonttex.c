#include "fonttex.h"

#define FTX_ERR "[fonttex error]: "
extern char *getFullPath(char*);

void getLine(char *buf, int size, FILE *f) {
  do {
    fgets(buf, size, f);
  } while( buf[0] == '\n' || buf[0] == '#');
}

fontbmp* fbmpLoadFont(char *filename) {
  char *path;
  FILE *file;
  char buf[100];
  int len;
  fontbmp *fbmp;
  
  path = getFullPath(filename);
  if(path == NULL) {
    fprintf(stderr, FTX_ERR "can't load font file '%s'\n", filename);
    return NULL;
  }
  file = fopen(path, "r");
  free(path);

  fbmp = (fontbmp*) malloc(sizeof(fontbmp));
  /* bitmap texture width, character width */
  getLine(buf, sizeof(buf), file);
  sscanf(buf, "%d %d ", &(fbmp->texwidth), &(fbmp->width));
  /* lowest character, highest character */
  getLine(buf, sizeof(buf), file);
  sscanf(buf, "%d %d ", &(fbmp->lower), &(fbmp->upper));
  /* bitmap filename */
  getLine(buf, sizeof(buf), file);
  len = strlen(buf) + 1;
  if(buf[len - 2] == '\n') buf[len - 2] = 0;
  /* fbmp->bitmapName = (char*) malloc(len);
     memcpy(fbmp->bitmapName, buf, len); */
  fbmp->tex = loadTextureData(buf);

  fclose(file);
  return fbmp;
}

fonttex *ftxLoadFont(char *filename) {
  char *path;
  FILE *file;
  char buf[100];

  int i;
  int len;
  fonttex *ftx;
  
  path = getFullPath(filename);
  if(path == NULL) {
    fprintf(stderr, FTX_ERR "can't load font file '%s'\n", filename);
    return NULL;
  }
  file = fopen(path, "r");
  free(path);

  /* TODO(5): check for EOF errors in the following code */
  
  /* nTextures, texture width, char width */
  ftx = (fonttex*) malloc(sizeof(fonttex));
  getLine(buf, sizeof(buf), file);
  sscanf(buf, "%d %d %d ", &(ftx->nTextures), &(ftx->texwidth), &(ftx->width));
  /* lowest character, highest character */
  getLine(buf, sizeof(buf), file);
  sscanf(buf, "%d %d ", &(ftx->lower), &(ftx->upper));
  /* font name */
  getLine(buf, sizeof(buf), file);
  len = strlen(buf) + 1;

  ftx->fontname = malloc(len);
  memcpy(ftx->fontname, buf, len);

  /* prepare space for texture IDs  */
  ftx->texID = (unsigned int*) malloc(ftx->nTextures * sizeof(unsigned int));
  glGenTextures(ftx->nTextures, ftx->texID);

  /* the individual textures */
  for(i = 0; i < ftx->nTextures; i++) {
    char *texname;
    getLine(buf, sizeof(buf), file);
    len = strlen(buf) + 1;
    if(buf[len - 2] == '\n') buf[len - 2] = 0;
    texname = (char*)malloc(len);
    memcpy(texname, buf, len); 
    glBindTexture(GL_TEXTURE_2D, ftx->texID[i]);
    loadTexture(texname, GL_RGBA);
    free(texname);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); */
    /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  fclose(file);
  return ftx;
}

void ftxUnloadFont(fonttex *ftx) {
  glDeleteTextures(ftx->nTextures, ftx->texID);

  free(ftx->texID);
  free(ftx->fontname);
  free(ftx);
}

void ftxRenderString(fonttex *ftx, char *string, int len) {
  int i;
  int bound = -1;
  int index;
  
  int tex;
  int w;
  float cw;
  float cx, cy;

  w = ftx->texwidth / ftx->width;
  cw = (float)ftx->width / (float)ftx->texwidth;

  for(i = 0; i < len; i++) {
    /* find out which texture it's in */
    /* TODO(4): find out why the +1 is necessary */
    index = string[i] - ftx->lower + 1;
    if(index >= ftx->upper) 
      fprintf(stderr, FTX_ERR " index out of bounds");
    tex = index / (w * 
w);
    /* bind texture */
    if(tex != bound) {
      glBindTexture(GL_TEXTURE_2D, ftx->texID[tex]);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      bound = tex;
    }
    /* find texture coordinates */
    index = index % (w * w);
    cx = (float)(index % w) / (float)w;
    cy = (float)(index / w) / (float)w;
    /* draw quad */
    /* fprintf(stderr, FTX_ERR "coords: tex %d (%.2f, %.2f), %.2f\n", */
    /*     bound, cx, cy, cw); */

    glBegin(GL_QUADS);
    glTexCoord2f(cx, 1 - cy - cw);
    glVertex2f(i, 0);
    glTexCoord2f(cx + cw, 1 - cy - cw);
    glVertex2f(i + 1, 0);
    glTexCoord2f(cx + cw, 1 - cy);
    glVertex2f(i + 1, 1);
    glTexCoord2f(cx, 1 - cy);
    glVertex2f(i, 1);
    glEnd();
  }
  /* checkGLError("fonttex.c ftxRenderString\n"); */
}



