#include "video/video.h"
#include "game/game.h"
#include "game/resource.h"
#include "filesystem/path.h"
#include "base/util.h"
#include "video/video_level.h"

#include "Nebu_scripting.h"
#include "Nebu_filesystem.h"

#include <string.h>
#include <assert.h>

#include <base/nebu_debug_memory.h>

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
      scripting_RunFormat("artpacks[%d] = \"%s\"", i, (char*) p->data);
	  free(p->data);
      i++;
  }
  nebu_List_Free(artList);
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
		char *path = nebu_FS_GetPath(PATH_ART, pHUDNames[i]);
		assert(!gpTokenHUD[i]);

		gpTokenHUD[i] = resource_GetToken(path, eRT_2d);
		if(!gpTokenHUD[i])
		{
			fprintf(stderr, "fatal: failed loading %s, exiting...\n", pHUDNames[i]);
			exit(1); /* OK: critical, installation corrupt */
		}
		free(path);
	}
}

void artpack_UnloadSurfaces()
{
	int i;

	for(i = 0; i < eHUDElementCount; i++)
	{
		if(gpTokenHUD[i])
			resource_Free(gpTokenHUD[i]);
		gpTokenHUD[i] = 0;
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
    

