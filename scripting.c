#include "scripting.h"

#include "lua.h"
#include "lualib.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static lua_State *L;
lua_State *L;

extern void init_c_interface(lua_State *L);

void scripting_Init() {
  L = lua_open(0);
  lua_baselibopen(L);
  lua_strlibopen(L);
  lua_iolibopen(L);

  init_c_interface(L);
}

void scripting_Quit() {
  lua_close(L);
}

int scripting_GetFloatSetting(char *name, float *f) {
  int status;
  lua_getglobal(L, "settings");
  lua_pushstring(L, name);
  lua_gettable(L, -2);

  if(lua_isnumber(L, -1)) {
    status = 0; /* all's well */
    *f = lua_tonumber(L, -1);
  } else
    status = 1;

  lua_pop(L, 2); /* restore stack */
  return status;
}


int scripting_GetIntegerSetting(char *name, int *i) {
  float f;
  int status;
  status = scripting_GetFloatSetting(name, &f);
  *i = f;
  return status;
}

int scripting_GetIntegerResult(int *i) {
  int status;
   if(lua_isnumber(L, -1)) {
    status = 0; /* all's well */
    *i = (int)lua_tonumber(L, -1);
  } else
    status = 1;

  lua_pop(L, 1); /* restore stack */
  return status;
}  

void scripting_SetFloatSetting(char *name, float f) {
  lua_getglobal(L, "settings");
  lua_pushstring(L, name);
  lua_pushnumber(L, f);
  lua_settable(L, -3);
  lua_pop(L, 1);
}

void scripting_GetFloatArray(char *name, float *f, int n) {
  int i;
  lua_getglobal(L, name);
  for(i = 0; i < n; i++) {
    lua_rawgeti(L, -1, i + 1);
    if(lua_isnumber(L, -1)) {
      *(f + i) = lua_tonumber(L, 2);
    } else {
      fprintf(stderr, "element %d is not number!\n", i);
    }
    lua_pop(L, 1);
  }
  lua_pop(L, 1); /* restore stack */
}

int scripting_GetStringSetting(char *name, char **s) {
  int status;
  lua_getglobal(L, "settings");
  lua_pushstring(L, name);
  lua_gettable(L, -2);
  status = scripting_GetStringResult(s);
  lua_pop(L, 1);
  return status;
}

int scripting_GetStringResult(char **s) {
  int status;
  if(lua_isstring(L, -1)) {
    int size;
    status = 0;
    size = lua_strlen(L, -1) + 1;
    *s = malloc( size );
    memcpy( *s, lua_tostring(L, -1), size );
    /* printf("allocated string '%s' of size %d\n", *s, size); */
  } else
    status = 1;

  lua_pop(L, 1);
  return status;
}

int scripting_CopyStringResult(char *s, int len) {
  int status;
  if(lua_isstring(L, -1)) {
    int size, copy;
    status = 0;
    size = lua_strlen(L, -1) + 1;
    if(size > len) { copy = len; status = 2; }
    else copy = size;
    memcpy( s, lua_tostring(L, -1), size );
  } else
    status = 1;

  lua_pop(L, 1);
  return status;
}    

void scripting_RunFile(char *name) {
  lua_dofile(L, name);
}

void scripting_Run(char *command) {
  /* fprintf(stderr, "[command] %s\n", command); */
  lua_dostring(L, command);
}

void scripting_RunFormat(char *format, ... ) {
  char buf[4096];
  va_list ap;
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);
  scripting_Run(buf);
}

void scripting_RunGC() {
  lua_setgcthreshold(L, 0);
}

void Scripting_Idle() {
	scripting_RunGC();
}
