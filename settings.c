#include "gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

/* this just screams to be replaced by scripting code */

void initDefaultSettings() {
  /* load some more defaults from config file */
  { 
    char *path;
    path = getPath(PATH_SCRIPTS, "config.lua");
    scripting_RunFile(path);
    free(path);
  }

  game->pauseflag = 0;

}

void initMainGameSettings() {
  game2 = &main_game2;
  game = &main_game;

  /* initialize some struct members */

  game2->input.mouse1 = 0;
  game2->input.mouse2 = 0;
  game2->input.mousex = 0;
  game2->input.mousey = 0;
  game2->network.status = 0;
}

void checkSettings() {
  /* sanity check: speed, grid_size */
  if(getSettingf("speed") <= 0) {
    fprintf(stderr, "[gltron] sanity check failed: speed = %.2ff\n",
	    getSettingf("speed"));
    setSettingf("speed", 6.0);
    fprintf(stderr, "[gltron] reset speed: speed = %.2f\n",
	    getSettingf("speed"));
  }
  if(getSettingi("grid_size") % 8) {
    fprintf(stderr, "[gltron] sanity check failed: grid_size %% 8 != 0: "
	    "grid_size = %d\n", getSettingi("grid_size"));
    setSettingi("grid_size", 240);
    fprintf(stderr, "[gltron] reset grid_size: grid_size = %d\n",
	    getSettingi("grid_size"));
  }
}

void saveSettings() {
  char *path;
  char *script;

  script = getPath(PATH_SCRIPTS, "save.lua");
  scripting_RunFile(script);
  free(script);

  path = getPossiblePath(PATH_PREFERENCES, RC_NAME);
  if(path != NULL) {
    scripting_RunFormat("writeto(\"%s\")", path);
    scripting_Run("save()");
    free(path);
  }
}

int getSettingi(char *name) {
  int value;
  if( scripting_GetIntegerSetting(name, &value) ) {
    /* does not exit, return default */
    fprintf(stderr, "no default known for '%s'!\n", name);
    assert(0);
    return 0;
  }

  lua_profile++;
  return value;
}

float getSettingf(char *name) {
  float value;
  if( scripting_GetFloatSetting(name, &value) ) {
    /* does not exit, return default */
    fprintf(stderr, "no default known for '%s'!\n", name);
    assert(0);
    return 0;
  }

  lua_profile++;
  return value;
}

void setSettingf(char *name, float f) {
  printf("setting '%s' to %.2f\n", name, f);
  scripting_SetFloatSetting( name, f );
}

void setSettingi(char *name, int i) {
  printf("setting '%s' to %d\n", name, i);
  scripting_SetFloatSetting( name, (float)i );
}

