#include "gltron.h"

void initArtpacks() {
  const char *art_path;
  list *artList;
  list *p;
  int i;

  art_path = getDirectory( PATH_ART );
  artList = readDirectoryContents(art_path, NULL);
  if(artList->next == NULL) {
    fprintf(stderr, "no art files found...exiting\n");
    exit(1); /* OK: critical, installation corrupt */
  }
  
  i = 1;
  for(p = artList; p->next != NULL; p = p->next) {
    scripting_RunFormat("artpacks[%d] = \"%s\"", i, (char*) p->data);
    i++;
  }
  scripting_Run("setupArtpacks()");
}

void loadArt() {
  char *path;
  char *artpack;

  scripting_GetStringSetting("current_artpack", &artpack);
  fprintf(stderr, "loading artpack '%s'\n", artpack);
  path = getArtPath(artpack, "artpack.ini");
  free(artpack);

  if(path != NULL) {
    scripting_RunFile(path);
    free(path);
  }

  initTexture(game->screen);
  initFonts();
}

void reloadArt() {
  printf("reloading art\n");
  deleteTextures(game->screen);
  loadArt();
}
    

