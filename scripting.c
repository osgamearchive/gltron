#include "lua.h"

static lua_State *L;

void scripting_Init() {
  L = lua_open(0);
}

void scripting_Quit() {
  lua_close(L);
}

void scripting_LoadConfig(char *name) {
  lua_dofile(L, name);
}

int scripting_GetFloat(char *name, float *f) {
  int status;
  lua_getglobal(L, name);
  if(lua_isnumber(L, 1)) {
    status = 0; /* all's well */
    *f = lua_tonumber(L, 1);
  } else
    status = 1;
  lua_pop(L, 1); /* restore stack */

  return status;
}


int scripting_GetInteger(char *name, int *i) {
  int status;
  lua_getglobal(L, name);
  if(lua_isnumber(L, 1)) {
    status = 0; /* all's well */
    *i = (int)lua_tonumber(L, 1);
  } else
    status = 1;
  lua_pop(L, 1); /* restore stack */

  return status;
}

void scripting_SetFloat(char *name, float f) {
  lua_pushnumber(L, f);
  lua_setglobal(L, name);
}









