#include "video/video.h"
#include "game/game.h"
#include "filesystem/path.h"
#include "base/util.h"
#include "video/video_level.h"

#include "Nebu_scripting.h"
#include "Nebu_filesystem.h"

#include <string.h>

void initArtpacks(void) {
  const char *art_path;
  nebu_List *artList;
  nebu_List *p;
  int i;

  art_path = getDirectory( PATH_ART );
  artList = readDirectoryContents(art_path, NULL);
  if(artList->next == NULL) {
    fprintf(stderr, "[fatal] no art files found...exiting\n");
    exit(1); /* OK: critical, installation corrupt */
  }
  
  i = 1;
  for(p = artList; p->next != NULL; p = p->next) {
    if(strncmp((char*)p->data, "Makefile", 8)) {
      scripting_RunFormat("artpacks[%d] = \"%s\"", i, (char*) p->data);
      i++;
    }
  }
  scripting_Run("setupArtpacks()");
}

void artpack_LoadSurfaces(void)
{
	// char *pHUDNames[eHUDElementCount] = {
	char *pHUDNames[] = {
		"hud-speed.png",
		"hud-mask-speed.png",
		"hud-mask-turbo.png",
		"hud-ai.png",
		"hud-map.png",
		"hud-scores.png",
		"hud-fps.png",
		"hud-buster.png",
		"hud-mask-buster.png"
	};
	int i;
	for(i = 0; i < eHUDElementCount; i++)
	{
		char *path;
		
		path = nebu_FS_GetPath(PATH_ART, pHUDNames[i]);
		if(!path)
		{
			fprintf(stderr, "fatal: failed loading %s, exiting...\n", pHUDNames[i]);
			exit(1); /* OK: critical, installation corrupt */
		}
		if(gpHUD[i]) { nebu_2d_Free(gpHUD[i]); gpHUD[i] = NULL; }
		gpHUD[i] = nebu_2d_LoadPNG(path, 0);

		free(path);
	}
}

void loadArt(void) {
	char *path;

	// load default art settings
	runScript(PATH_SCRIPTS, "artpack.lua");

	// load custom artpack settings
	path = nebu_FS_GetPath(PATH_ART, "artpack.lua");
	if(path != NULL) {
		scripting_RunFile(path);
		free(path);
	}

	initTexture(gScreen);
	fprintf(stderr, "[status] done loading textures...\n");
	initFonts();
	fprintf(stderr, "[status] done loading fonts...\n");
	
	artpack_LoadSurfaces();

	video_LoadLevel();
	fprintf(stderr, "[status] done loading level...\n");
}

void reloadArt(void) {
  printf("[status] reloading art\n");
  deleteTextures(gScreen);
  loadArt();
}
    

