#include "video/video.h"
#include "filesystem/path.h"
#include "Nebu_filesystem.h"

void initFonts(void) {
  char *path;
  file_handle file;
  char buf[100];
  char gamefont[100];
  char *game = NULL;

  if(gameFtx != NULL) nebu_Font_Free(gameFtx);

  path = getPath(PATH_DATA, "fonts.txt");
  if(path != NULL) {
    file = file_open(path, "r");
    while(file_gets(file, buf, sizeof(buf)) != NULL) {
      if(sscanf(buf, "game: %s ", gamefont) == 1)
		game = gamefont;
    }
    file_close(file);
    free(path);
  } else {
    fprintf(stderr, "can't load fonts.txt\n");
    exit(1); /* OK: critical, installation corrupt */
  }

  if(game == NULL) {
    fprintf(stderr, "incomplete font definition in fonts.txt\n");
    exit(1); /* OK: critical, installation corrupt */
  }

  path = getPath(PATH_DATA, game);
  if(path != NULL) {
	  gameFtx = nebu_Font_Load(path, PATH_ART);
	  free(path);
  }

  if(gameFtx == NULL) {
    fprintf(stderr, "can't load font %s\n", game);
    exit(1); /* OK: critical, installation corrupt */
  }
}

void deleteFonts(void) {
  if(gameFtx != NULL)
    nebu_Font_Free(gameFtx);
  gameFtx = NULL;
}
