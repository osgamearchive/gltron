#include "gltron.h"

void initFonts() {
  if(gameFtx != NULL) ftxUnloadFont(gameFtx);
  if(guiFtx != NULL) ftxUnloadFont(guiFtx);
  gameFtx = ftxLoadFont("xenotron.ftx");
  guiFtx = ftxLoadFont("babbage.ftx");
  
  if(gameFtx == NULL) {
    fprintf(stderr, "can't load font xenotron.ftx\n");
    exit(1);
  }

  if(guiFtx == NULL) {
    fprintf(stderr, "can't load font babbage.ftx\n");
    exit(1);
  }

  fprintf(stderr, "initFonts end\n");
  // ftxEstablishTexture(ftx, GL_TRUE);
}

void deleteFonts() {
  if(gameFtx != NULL)
    ftxUnloadFont(gameFtx);
  gameFtx = NULL;
  if(guiFtx != NULL)
    ftxUnloadFont(guiFtx);
  guiFtx = NULL;
}
