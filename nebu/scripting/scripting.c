#include "nebu_scripting.h"

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

  // init_c_interface(L);
}

void scripting_Quit() {
  lua_close(L);
}


void showStack() {
	int i;
	printf("dumping stack with %d elements\n", lua_gettop(L));
	for(i = 0; i < lua_gettop(L); i++) {
		int type = lua_type(L, - (i+1));
		switch(type) {
		case LUA_TNIL: printf("nil\n"); break;
		case LUA_TNUMBER: printf("number\n"); break;
		case LUA_TSTRING: printf("string\n"); break;
		case LUA_TTABLE: printf("table\n"); break;
		case LUA_TFUNCTION: printf("function\n"); break;
		case LUA_TUSERDATA: printf("userdata\n"); break;
		}
	}
}

int scripting_IsNilResult() {
	int result = lua_isnil(L, -1);
	lua_pop(L, 1);
	return result;
}

int getGlobal(const char *s, va_list ap) {
	int top = lua_gettop(L);
	int count = 0;
	while(s) {
		lua_pushstring(L, s);
		lua_gettable(L, -2);
		count++;
		s = va_arg(ap, char *);
	}
	lua_insert(L, top); /* move result to bottom */
	lua_pop(L, count); /* restore stack */
	return 0;
}

int scripting_GetGlobal(const char *global, const char *s, ...) {
	lua_getglobal(L, global);
	if(s) {
		va_list ap;
		va_start(ap, s);
		getGlobal(s, ap);
		va_end(ap);
	}
	return 0;
}	

int scripting_SetFloat(float f, const char *name, const char *global, const char *s, ...) {
	va_list ap;
	
	if(global == NULL) {
		lua_pushnumber(L, f);
		lua_setglobal(L, global);
		return 0;
	}
	
	lua_getglobal(L, global);
	
	if(s) {
		va_start(ap, s);
		getGlobal(s, ap);
		va_end(ap);
	}
	
	lua_pushstring(L, name);
	lua_pushnumber(L, f);
	lua_settable(L, -3);
	lua_pop(L, 1);

	return 0;
}

int scripting_GetFloatResult(float *f) {
	if(lua_isnumber(L, -1)) {
    *f = lua_tonumber(L, -1);
		lua_pop(L, 1); /* restore stack */
		return 0;
	} else {
		showStack();
    return 1;
	}
}  

int scripting_GetIntegerResult(int *i) {
	if(lua_isnumber(L, -1)) {
    *i = (int)lua_tonumber(L, -1);
		lua_pop(L, 1); /* restore stack */
		return 0;
	} else {
		showStack();
		return 1;
	}
}  

void scripting_GetFloatArrayResult(float *f, int n) {
  int i;
	
  for(i = 0; i < n; i++) {
    lua_rawgeti(L, -1, i + 1);
    if(lua_isnumber(L, -1)) {
      *(f + i) = (float)lua_tonumber(L, 2);
    } else {
      fprintf(stderr, "element %d is not number!\n", i);
    }
    lua_pop(L, 1); /* remove number from stack */
  }

	lua_pop(L, 1); /* remove table from stack */
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

void scripting_Register(const char *name, int(*func) (lua_State *L)) {
	lua_register(L, name, func);
}
