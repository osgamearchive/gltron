#include "gltron.h"

char *gameDefault;
void initFonts() {
  char *path;
  FILE *f;
  char buf[100];
  char gamefont[100];
  char guifont[100];
  char *game, *gui;

  if(gameFtx != NULL) ftxUnloadFont(gameFtx);
  if(guiFtx != NULL) ftxUnloadFont(guiFtx);

  path = getFullPath("fonts.txt");
  if(path != NULL) {
    f = fopen(path, "r");
    while(fgets(buf, sizeof(buf), f) != NULL) {
      if(sscanf(buf, "game: %s ", gamefont) == 1)
	game = gamefont;
      else if(sscanf(buf, "menu: %s ", guifont) == 1)
	gui = guifont;
    }
  }
  free(path);

  gameFtx = ftxLoadFont(game);
  guiFtx = ftxLoadFont(gui);

  if(gameFtx == NULL) {
    fprintf(stderr, "can't load font %s\n", game);
    exit(1);
  }

  if(guiFtx == NULL) {
    fprintf(stderr, "can't load font %s\n", gui);
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
