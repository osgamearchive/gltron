#include "gltron.h"

void initFonts() {
  if(ftx != NULL) ftxUnloadFont(ftx);
  ftx = ftxLoadFont("xenotron.ftx");
  
  if(ftx == NULL) {
    fprintf(stderr, "can't load font xenotron.ftx\n");
    exit(1);
  }
  ftxEstablishTexture(ftx, GL_TRUE);
}

void deleteFonts() {
  if(ftx != NULL)
    ftxUnloadFont(ftx);
  ftx = NULL;
}
