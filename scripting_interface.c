#include "gltron.h"

#include "lua.h"
#include "lualib.h"

int c_quitGame(lua_State *L) {
  saveSettings();
  switchCallbacks(&creditsCallbacks);
  return 0;
}

int c_resetGame(lua_State *L) {
  initData();
}

int c_resetScores(lua_State *L) {
  printf("c_resetScores called");
  resetScores();
  return 0;
}

int c_video_restart(lua_State *L) {
  initGameScreen();
  shutdownDisplay( game->screen );
  setupDisplay( game->screen );
  updateCallbacks();
  changeDisplay();
  return 0;
}

int c_update_audio_volume(lua_State *L) { 
#ifdef SOUND
 setMusicVolume(getSettingf("musicVolume"));
 setFxVolume(getSettingf("fxVolume"));
#endif
 return 0;
}

int c_startGame(lua_State *L) { 
  game2->mode = GAME_SINGLE;
  initData();
  changeDisplay();
  switchCallbacks(&pauseCallbacks);
  return 0;
}

int c_reloadMusic(lua_State *L) {
#ifdef SOUND
  char *tmp, *path;
#if 0
  path = getPath(PATH_DATA, MUSIC_DIR);
  tmp = (char*)malloc(strlen(path) + 1 + /* seperator */
		      strlen((char*) p->data) + 1);
  sprintf(tmp, "%s%c%s", path, SEPERATOR, 
	  (char*) p->data);
  fprintf(stderr, "loading song %s\n", tmp);

  if(getSettingi("playMusic"))
    stopSound();
  loadSound(tmp);
  free(tmp);
  if(getSettingi("playMusic"))
    playSound();
#endif

#endif
  return 0;
}

int c_reloadTextures(lua_State *L) {
  reloadArt();
  return 0;
}
  
int c_restoreDefaults(lua_State *L) {
  initDefaultSettings();
  c_video_restart(L);
  fprintf(stderr, "loaded default settings\n");
  return 0;
}

void init_c_interface(lua_State *L) {
  lua_register(L, "c_quitGame", c_quitGame);
  lua_register(L, "c_resetGame", c_resetGame);
  lua_register(L, "c_resetScores", c_resetScores);
  lua_register(L, "c_video_restart", c_video_restart);
  lua_register(L, "c_update_audio_volume", c_update_audio_volume);
  lua_register(L, "c_startGame", c_startGame);
  lua_register(L, "c_reloadMusic", c_reloadMusic);
  lua_register(L, "c_reloadTextures", c_reloadTextures);
  lua_register(L, "c_restoreDefaults", c_restoreDefaults);
}

