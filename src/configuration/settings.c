#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base/util.h"
#include "game/gltron.h"
#include "filesystem/path.h"

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

/* this just screams to be replaced by scripting code */

void initDefaultSettings() {
  /* load some more defaults from config file */
	runScript(PATH_SCRIPTS, "config.lua");
	runScript(PATH_SCRIPTS, "artpack.lua");

  game->pauseflag = 0;

}

void initMainGameSettings() {
  game2 = &main_game2;
  game = &main_game;

  /* initialize some struct members */

  gInput.mouse1 = 0;
  gInput.mouse2 = 0;
  gInput.mousex = 0;
  gInput.mousey = 0;
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
		scripting_Run("writeto()"); // select stdout again
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

  return value;
}

void setSettingf(char *name, float f) {
  // printf("setting '%s' to %.2f\n", name, f);
  scripting_SetFloatSetting( name, f );
}

void setSettingi(char *name, int i) {
  // printf("setting '%s' to %d\n", name, i);
  scripting_SetFloatSetting( name, (float)i );
}

void updateSettingsCache() {
  /* cache lua settings that don't change during play */
  game2->settingsCache.use_stencil = getSettingi("use_stencil");
  game2->settingsCache.show_scores = getSettingi("show_scores");
  game2->settingsCache.show_ai_status = getSettingi("show_ai_status");
  game2->settingsCache.ai_level = getSettingi("ai_level");
  game2->settingsCache.show_fps = getSettingi("show_fps");
  game2->settingsCache.show_console = getSettingi("show_console");
  game2->settingsCache.softwareRendering = getSettingi("softwareRendering");
  game2->settingsCache.show_floor_texture = getSettingi("show_floor_texture");
  game2->settingsCache.line_spacing = getSettingi("line_spacing");
  game2->settingsCache.show_decals = getSettingi("show_decals");
  game2->settingsCache.alpha_trails = getSettingi("alpha_trails");
  game2->settingsCache.antialias_lines = getSettingi("antialias_lines");
  game2->settingsCache.turn_cycle = getSettingi("turn_cycle"); 
  game2->settingsCache.light_cycles = getSettingi("light_cycles"); 
  game2->settingsCache.lod = getSettingi("lod"); 
  game2->settingsCache.fov = getSettingi("fov"); 
  game2->settingsCache.stretch_textures = getSettingi("stretch_textures"); 
  game2->settingsCache.show_skybox = getSettingi("show_skybox"); 
  game2->settingsCache.show_recognizer = getSettingi("show_recognizer");
  game2->settingsCache.show_impact = getSettingi("show_impact");
  game2->settingsCache.show_glow = getSettingi("show_glow"); 
  game2->settingsCache.show_wall = getSettingi("show_wall");
  game2->settingsCache.fast_finish = getSettingi("fast_finish");
  game2->settingsCache.fov = getSettingf("fov");
  game2->settingsCache.znear = getSettingf("znear");
  game2->settingsCache.camType = getSettingi("camType");
  game2->settingsCache.playEffects = getSettingi("playEffects");
  game2->settingsCache.playMusic = getSettingi("playMusic");
	game2->settingsCache.map_ratio_w = getSettingf("map_ratio_w");
	game2->settingsCache.map_ratio_h = getSettingf("map_ratio_h");
	
  scripting_GetFloatArray("clear_color", game2->settingsCache.clear_color, 4);
}
