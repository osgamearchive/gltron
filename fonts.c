#include "gltron.h"

char *gameDefault;
void initFonts() {
  char *path;
  FILE *f;
  char buf[100];
  char gamefont[100];
  char guifont[100];
  char netfont[100];
  char *game = NULL, *gui = NULL, *net = NULL;

  if(gameFtx != NULL) ftxUnloadFont(gameFtx);
  if(guiFtx != NULL) ftxUnloadFont(guiFtx);
  if(netFtx != NULL) ftxUnloadFont(netFtx);

  path = getFullPath("fonts.txt");
  if(path != NULL) {
    f = fopen(path, "r");
    while(fgets(buf, sizeof(buf), f) != NULL) {
      if(sscanf(buf, "game: %s ", gamefont) == 1)
	game = gamefont;
      else if(sscanf(buf, "menu: %s ", guifont) == 1)
	gui = guifont;
      else if(sscanf(buf, "net: %s ", netfont) == 1)
	net = netfont;
    }
    fclose(f);
    free(path);
  } else {
    fprintf(stderr, "can't load fonts.txt\n");
    exit(1);
  }

  if(game == NULL || gui == NULL || net == NULL) {
    fprintf(stderr, "incomplete font definition in fonts.txt\n");
    exit(1);
  }

  gameFtx = ftxLoadFont(game);
  guiFtx = ftxLoadFont(gui);
  netFtx = ftxLoadFont(net);

  if(gameFtx == NULL) {
    fprintf(stderr, "can't load font %s\n", game);
    exit(1);
  }

  if(guiFtx == NULL) {
    fprintf(stderr, "can't load font %s\n", gui);
    exit(1);
  }

  if(netFtx == NULL) {
    fprintf(stderr, "can't load font %s\n", net);
    exit(1);
  }
}

void deleteFonts() {
  if(gameFtx != NULL)
    ftxUnloadFont(gameFtx);
  gameFtx = NULL;
  if(guiFtx != NULL)
    ftxUnloadFont(guiFtx);
  guiFtx = NULL;
  if(netFtx != NULL)
    ftxUnloadFont(netFtx);
  netFtx = NULL;
}
