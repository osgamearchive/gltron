#include "lua.h"
#include "lualib.h"

#include <stdio.h>

static lua_State *L;

void scripting_Init() {
  L = lua_open(0);
  lua_baselibopen(L);
  lua_strlibopen(L);
  lua_iolibopen(L);
}

void scripting_Quit() {
  lua_close(L);
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

void scripting_GetFloatArray(char *name, float *f, int n) {
  int i;
  lua_getglobal(L, name);
  for(i = 0; i < n; i++) {
    lua_rawgeti(L, 1, i + 1);
    if(lua_isnumber(L, 2)) {
      *(f + i) = lua_tonumber(L, 2);
    } else {
      fprintf(stderr, "element %d is not number!\n", i);
    }
    lua_pop(L, 1);
  }
  lua_pop(L, 1); /* restore stack */
}

void scripting_DoFile(char *name) {
  lua_dofile(L, name);
}

void scripting_DoString(char *command) {
  lua_dostring(L, command);
}
