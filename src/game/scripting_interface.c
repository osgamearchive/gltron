#include "game/gltron.h"
#include "base/switchCallbacks.h"
#include "scripting/nebu_scripting.h"
#include "filesystem/path.h"

#include "audio/audio.h"

#include "lua.h"
#include "lualib.h"

int c_quitGame(lua_State *L) {
  saveSettings();
	SystemExitLoop(RETURN_CREDITS);
  return 0;
}

int c_resetGame(lua_State *L) {
  game_ResetData();
	video_ResetData();
  return 0;
}

int c_resetScores(lua_State *L) {
  resetScores();
  return 0;
}

int c_resetCamera(lua_State *L) {
  int i;
  int camType;
  Camera *cam;
  Data *data;

  for(i = 0; i < game->players; i++) {
    cam = game->player[i].camera;
    data = game->player[i].data;

    camType = (game->player[i].ai->active == AI_COMPUTER) ? 
      0 : getSettingi("camType");
    initCamera(cam, data, camType);
  }
  return 0;
}

int c_video_restart(lua_State *L) {
  initGameScreen();
  shutdownDisplay( gScreen );
  setupDisplay( gScreen );
  updateCallbacks();
  changeDisplay(-1);
  return 0;
}

int c_update_settings_cache(lua_State *L) {
  updateSettingsCache();
  return 0;
}

int c_update_audio_volume(lua_State *L) { 
 Sound_setMusicVolume(getSettingf("musicVolume"));
 Sound_setFxVolume(getSettingf("fxVolume"));
 return 0;
}

int c_startGame(lua_State *L) { 
  game2->mode = GAME_SINGLE;
	game_ResetData();
	video_ResetData();
  changeDisplay(-1);
	SystemExitLoop(RETURN_GAME_LAUNCH);
  return 0;
}

int c_reloadTrack(lua_State *L) {
  Sound_reloadTrack();
  return 0;
}

int c_reloadArtpack(lua_State *L) {
  reloadArt();
  return 0;
}

int c_reloadLevel(lua_State *L) {
  loadLevel();
	reloadArt();
	initGameLevel();
	game_ResetData();
	video_ResetData(); // already called by reloadArt()
  return 0;
}
  
int c_configureKeyboard(lua_State *L) {
	SystemExitLoop(RETURN_GUI_PROMPT);
  return 0;
}

int c_getKeyName(lua_State *L) {
  int top = lua_gettop(L);
  if(lua_isnumber(L, top)) {
    lua_pushstring(L, SystemGetKeyName( (int) lua_tonumber(L, top) ));
  } else {
    lua_pushstring(L, "error");
  }
  return 1;
}

int c_timedemo(lua_State *L) {
	SystemExitLoop(RETURN_TIMEDEMO);
	return 0;
}

int c_SetCallback(lua_State *L) {
	const char *name;
	int top = lua_gettop(L);
	if(lua_isstring(L, top)) {
		name = lua_tostring(L, top);
		setCallback(name);
		// printf("enabling callback-set '%s'\n", name);
	}
	return 0;
}

int c_SystemMainLoop(lua_State *L) {
	int value = SystemMainLoop();
	lua_pushnumber(L, value);

	return 1;
}

int c_loadDirectory(lua_State *L) {
	int dir;
	char *dirPath;
	nebu_List *files, *p;
	int nFiles = 0;

	// load directory enum from stack
	int top = lua_gettop(L); // number of arguments
	if(top != 1) {
		// wrong number of arguments
		lua_error(L, "wrong number of arguments for function "
							"c_loadDirectory: should be 1\n");
	}
	if(!lua_isnumber(L, -1)) {
		lua_error(L, "number  expected for arg1 to function "
							"c_loadDirecotry");
	}
	dir = lua_tonumber(L, -1);

	dirPath = getDirectory(dir); // PATH_ART or PATH_LEVEL or PATH_MUSIC
	files = readDirectoryContents(dirPath, NULL);

	lua_newtable(L);
	for(p = files; p->next; p = p->next) {
		lua_pushstring(L, p->data);
		lua_rawseti(L, -2, nFiles + 1);
		nFiles++;
	}
	/* FIXME: when does the list get freed */
	return 1;
}

void init_c_interface(void) {
  scripting_Register("c_quitGame", c_quitGame);
  scripting_Register("c_resetGame", c_resetGame);
  scripting_Register("c_resetScores", c_resetScores);
  scripting_Register("c_resetCamera", c_resetCamera);
  scripting_Register("c_video_restart", c_video_restart);
  scripting_Register("c_update_settings_cache", c_update_settings_cache);
  scripting_Register("c_update_audio_volume", c_update_audio_volume);
  scripting_Register("c_startGame", c_startGame);
  scripting_Register("c_reloadTrack", c_reloadTrack);
  scripting_Register("c_reloadArtpack", c_reloadArtpack);
  scripting_Register("c_reloadLevel", c_reloadLevel);
  scripting_Register("c_configureKeyboard", c_configureKeyboard);
  scripting_Register("c_getKeyName", c_getKeyName);
	scripting_Register("c_timedemo", c_timedemo);
	scripting_Register("c_loadDirectory", c_loadDirectory);
	scripting_Register("SystemMainLoop", c_SystemMainLoop);
	scripting_Register("SetCallback", c_SetCallback);
}
