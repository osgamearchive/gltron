extern void scripting_Init();
extern void scripting_Quit();

extern int scripting_GetFloat(char *name, float *f);
extern int scripting_GetInteger(char *name, int *i);
extern void scripting_GetFloatArray(char *name, float *f, int n);

extern void scripting_SetFloat(char *name, float f);
extern void scripting_DoFile(char *name);
extern void scripting_DoString(char *command);


