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
  return 0;
}

int c_resetScores(lua_State *L) {
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

int c_reloadTrack(lua_State *L) {
  reloadTrack();
  return 0;
}

int c_reloadArtpack(lua_State *L) {
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
  lua_register(L, "c_reloadTrack", c_reloadTrack);
  lua_register(L, "c_reloadArtpack", c_reloadArtpack);
  lua_register(L, "c_restoreDefaults", c_restoreDefaults);
}





