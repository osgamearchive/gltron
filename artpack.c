#include "gltron.h"

char **artpack_list;
int artpack_index = 0;

void initArtpacks() {
  list *l, *p;
  int i = 0, n = 0;

  l = readDirectoryContents(PATH_ART, NULL);
  for(p = l; p->next != NULL; p = p->next)
    n++;

  artpack_list = (char**) malloc((n + 1) * sizeof(char*));
  
  for(p = l; p->next != NULL; p = p->next) {
    artpack_list[i] = (char*) malloc(strlen( (char*) p->data ) + 1);
    memcpy(artpack_list[i], p->data, strlen( (char*) p->data ) + 1);
    printf("loading artpack %s\n", artpack_list[i]);
    if(strstr(artpack_list[i], "default") == artpack_list[i])
      artpack_index = i;
    i++;

  }
  printf("loaded %d artpacks\n", i);
  artpack_list[i] = NULL;
}

void loadArt() {
  char *path;
  char *artpack;

  artpack = artpack_list[artpack_index];

  path = getArtPath(artpack, "artpack.ini");
  if(path != NULL) {
    scripting_DoFile(path);
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
    

