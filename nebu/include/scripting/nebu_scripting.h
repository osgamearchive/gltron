#ifndef NEBU_SCRIPTING_H
#define NEBU_SCRIPTING_H

#include "lua.h"

extern void scripting_Init();
extern void scripting_Quit();
extern void Scripting_Idle();

extern int scripting_GetFloatSetting(const char *name, float *f);
extern int scripting_GetIntegerSetting(const char *name, int *i);
extern int scripting_GetIntegerResult(int *i);
extern void scripting_GetFloatArray(const char *name, float *f, int n);
extern int scripting_GetStringSetting(const char *name, char **s);
extern int scripting_GetStringResult(char **s);
extern int scripting_CopyStringResult(char *s, int len);

extern void scripting_SetFloatSetting(const char *name, float f);
extern void scripting_RunFile(const char *name);
extern void scripting_Run(const char *command);
extern void scripting_RunFormat(const char *format, ...);
extern void scripting_RunGC();
extern void scripting_Register(const char *name, int(*func) (lua_State *L));

#endif
