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
  lua_getglobal(L, name);
  return scripting_GetIntegerResult(i);
}

int scripting_GetIntegerResult(int *i) {
  int status;
  int top = lua_gettop(L);
   if(lua_isnumber(L, top)) {
    status = 0; /* all's well */
    *i = (int)lua_tonumber(L, top);
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

int scripting_GetString(char *name, char **s) {
  lua_getglobal(L, name);
  return scripting_GetStringResult(s);
}

int scripting_GetStringResult(char **s) {
  int status;
  int top = lua_gettop(L);
  if(lua_isstring(L, top)) {
    int size;
    status = 0;
    size = lua_strlen(L, top) + 1;
    *s = malloc( size );
    memcpy( *s, lua_tostring(L, top), size );
    /* printf("allocated string '%s' of size %d\n", *s, size); */
  } else
    status = 1;

  lua_pop(L, 1);
  return status;
}

int scripting_CopyStringResult(char *s, int len) {
  int status;
  int top = lua_gettop(L);
  if(lua_isstring(L, top)) {
    int size, copy;
    status = 0;
    size = lua_strlen(L, top) + 1;
    if(size > len) { copy = len; status = 2; }
    else copy = size;
    memcpy( s, lua_tostring(L, top), size );
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
