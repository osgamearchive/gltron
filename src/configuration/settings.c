#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base/util.h"
#include "game/gltron.h"
#include "filesystem/path.h"

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

void checkSettings(void) {
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

void saveSettings(void) {
	char *script;
	script = getPath(PATH_SCRIPTS, "save.lua");
	scripting_RunFile(script);
	free(script);

#ifdef WIN32
	scripting_RunFormat("writeto(\"%s\")", "gltron.ini");
#else
	{
		char *path = getPossiblePath(PATH_PREFERENCES, RC_NAME);
		if(path == NULL)
			return;
		scripting_RunFormat("writeto(\"%s\")", path);
		free(path);
	}
#endif

	scripting_Run("save()");
	scripting_Run("writeto()"); // select stdout again
}

int getSettingi(const char *name) {
  int value;
  if( scripting_GetIntegerSetting(name, &value) ) {
    /* does not exit, return default */
    fprintf(stderr, "no default known for '%s'!\n", name);
    assert(0);
    return 0;
  }

  return value;
}

float getSettingf(const char *name) {
  float value;
  if( scripting_GetFloatSetting(name, &value) ) {
    /* does not exit, return default */
    fprintf(stderr, "no default known for '%s'!\n", name);
    assert(0);
    return 0;
  }

  return value;
}

int isSettingf(const char *name) {
	float value;
	return (scripting_GetFloatSetting(name, &value) == 0);
}

int isSettingi(const char *name) {
	int value;
	return (scripting_GetIntegerSetting(name, &value) == 0);
}

void setSettingf(const char *name, float f) {
  // printf("setting '%s' to %.2f\n", name, f);
  scripting_SetFloatSetting( name, f );
}

void setSettingi(const char *name, int i) {
  // printf("setting '%s' to %d\n", name, i);
  scripting_SetFloatSetting( name, (float)i );
}

void updateSettingsCache(void) {
  /* cache lua settings that don't change during play */
  gSettingsCache.use_stencil = getSettingi("use_stencil");
  gSettingsCache.show_scores = getSettingi("show_scores");
  gSettingsCache.show_ai_status = getSettingi("show_ai_status");
  gSettingsCache.ai_level = getSettingi("ai_level");
  gSettingsCache.show_fps = getSettingi("show_fps");
  gSettingsCache.show_console = getSettingi("show_console");
  gSettingsCache.softwareRendering = getSettingi("softwareRendering");
  gSettingsCache.show_floor_texture = getSettingi("show_floor_texture");
  gSettingsCache.line_spacing = getSettingi("line_spacing");
  gSettingsCache.show_decals = getSettingi("show_decals");
  gSettingsCache.alpha_trails = getSettingi("alpha_trails");
  gSettingsCache.antialias_lines = getSettingi("antialias_lines");
  gSettingsCache.turn_cycle = getSettingi("turn_cycle"); 
  gSettingsCache.light_cycles = getSettingi("light_cycles"); 
  gSettingsCache.lod = getSettingi("lod"); 
  gSettingsCache.fov = getSettingi("fov"); 
  gSettingsCache.stretch_textures = getSettingi("stretch_textures"); 
  gSettingsCache.show_skybox = getSettingi("show_skybox"); 
  gSettingsCache.show_recognizer = getSettingi("show_recognizer");
  gSettingsCache.show_impact = getSettingi("show_impact");
  gSettingsCache.show_glow = getSettingi("show_glow"); 
  gSettingsCache.show_wall = getSettingi("show_wall");
  gSettingsCache.fast_finish = getSettingi("fast_finish");
  gSettingsCache.fov = getSettingf("fov");
  gSettingsCache.znear = getSettingf("znear");
  gSettingsCache.camType = getSettingi("camType");
  gSettingsCache.playEffects = getSettingi("playEffects");
  gSettingsCache.playMusic = getSettingi("playMusic");
	gSettingsCache.map_ratio_w = getSettingf("map_ratio_w");
	gSettingsCache.map_ratio_h = getSettingf("map_ratio_h");
	
  scripting_GetFloatArray("clear_color", gSettingsCache.clear_color, 4);
}
