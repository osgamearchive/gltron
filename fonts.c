#include "gltron.h"

void initFonts() {
  if(ftx != NULL) ftxUnloadFont(ftx);
  ftx = ftxLoadFont("babbage.ftx");
  
  if(ftx == NULL) {
    fprintf(stderr, "can't load font xenotron.ftx\n");
    exit(1);
  }
  fprintf(stderr, "initFonts end\n");
  // ftxEstablishTexture(ftx, GL_TRUE);
}

void deleteFonts() {
  if(ftx != NULL)
    ftxUnloadFont(ftx);
  ftx = NULL;
}
