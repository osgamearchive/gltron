#include "gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

/* this just screams to be replaced by scripting code */

void initDefaultSettings() {
  game->pauseflag = 0;

  setSettingi("show_help", 0);
  setSettingi("show_fps", 0);
  setSettingi("show_wall", 1);
  setSettingi("show_glow", 1);
  setSettingi("show_2d", 0);
  setSettingi("show_alpha", 1);
  setSettingi("light_cycles", 1);
  setSettingi("alpha_trails", 0);
  setSettingi("show_skybox", 0);
  setSettingi("show_floor_texture", 1);
  setSettingi("show_crash_texture", 1);
  setSettingi("show_decals", 1);
  setSettingi("show_model", 1);
  setSettingi("show_recognizer", 0);
  setSettingi("lod", 0);
  setSettingi("shadow_lod", 2);
  setSettingi("turn_cycle", 1);
  setSettingi("line_spacing", 20);
  setSettingi("erase_crashed", 1);
  setSettingi("fast_finish", 1);
  setSettingi("fov", 105);
  setSettingf("znear", 0.5);
  setSettingf("current_speed", 7.0);
  setSettingi("game_speed", 2); /* fast */
  /* getSettingi("grid_size") MUST be divisible by 8 */
  setSettingi("arena_size", 3);
  setSettingi("grid_size", 600);
  setSettingi("width", 640);
  setSettingi("height", 480);
  setSettingi("show_ai_status", 0);
  setSettingi("show_scores", 1);
  setSettingi("show_gl_logo", 1);
  setSettingi("camType", 1);
  setSettingi("mouse_warp", 0);
  setSettingi("windowMode", 0);
  setSettingi("model_backwards", 0);
  setSettingi("use_mipmaps", 1);
  setSettingi("mipmap_filter", TRILINEAR);
  setSettingi("stretch_textures", 0);
  setSettingi("softwareRendering", 0);
  setSettingi("bitdepth_32", 0);

  setSettingi("antialias_lines", 1);

  setSettingi("display_type", 3);
  setSettingi("playMusic", 1);
  setSettingi("playEffects", 1);

  setSettingf("musicVolume", 0.3);
  setSettingf("fxVolume", 0.7);

  setSettingi("ai_player1", 0);
  setSettingi("ai_player2", 1);
  setSettingi("ai_player3", 1);
  setSettingi("ai_player4", 1);

  setSettingi("ai_level", 2);
  
  /* not included in .gltronrc */

  setSettingi("screenSaver", 0);

  viewport_content[0] = 0;
  viewport_content[1] = 1;
  viewport_content[2] = 2;
  viewport_content[3] = 3;

  setSettingi("grid_size", default_arena_sizes[getSettingi("arena_size")]);

  /* choose speed */
  default_speeds[4] = getSettingf("current_speed");
  setSettingf("current_speed", default_speeds[ getSettingi("game_speed") ]);
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

void initColors() {
  scripting_GetFloatArray("model_diffuse_0", colors_model_diffuse[0], 4);
  scripting_GetFloatArray("model_diffuse_1", colors_model_diffuse[1], 4);
  scripting_GetFloatArray("model_diffuse_2", colors_model_diffuse[2], 4);
  scripting_GetFloatArray("model_diffuse_3", colors_model_diffuse[3], 4);

  scripting_GetFloatArray("model_specular_0", colors_model_specular[0], 4);
  scripting_GetFloatArray("model_specular_1", colors_model_specular[1], 4);
  scripting_GetFloatArray("model_specular_2", colors_model_specular[2], 4);
  scripting_GetFloatArray("model_specular_3", colors_model_specular[3], 4);

  scripting_GetFloatArray("trail_diffuse_0", colors_trail_diffuse[0], 4);
  scripting_GetFloatArray("trail_diffuse_1", colors_trail_diffuse[1], 4);
  scripting_GetFloatArray("trail_diffuse_2", colors_trail_diffuse[2], 4);
  scripting_GetFloatArray("trail_diffuse_3", colors_trail_diffuse[3], 4);

}

void checkSettings() {
  /* sanity check: speed, grid_size */
  if(getSettingf("current_speed") <= 0) {
    fprintf(stderr, "[gltron] sanity check failed: speed = %.2ff\n",
	    getSettingf("current_speed"));
    setSettingf("current_speed", 6.0);
    fprintf(stderr, "[gltron] reset speed: speed = %.2f\n",
	    getSettingf("current_speed"));
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
  scripting_DoFile(script);
  free(script);

  path = getPossiblePath(PATH_PREFERENCES, RC_NAME);
  if(path != NULL) {
    char command[] = "writeto(\"%s\")";
    char buf[260];
    sprintf(buf, command, path);


    scripting_DoString(buf);
    scripting_DoString("save()");
    free(path);
  }
}

int getSettingi(char *name) {
  int value;
  if( scripting_GetInteger(name, &value) ) {
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
  if( scripting_GetFloat(name, &value) ) {
    /* does not exit, return default */
    fprintf(stderr, "no default known for '%s'!\n", name);
    assert(0);
    return 0;
  }

  lua_profile++;
  return value;
}

void setSettingf(char *name, float f) {
  scripting_SetFloat( name, f );
  printf("setting '%s' to %.2f\n", name, f);
}

void setSettingi(char *name, int i) {
  scripting_SetFloat( name, (float)i );
  printf("setting '%s' to %d\n", name, i);
}

