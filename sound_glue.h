
#ifdef __cplusplus
extern "C" {
#endif

void Audio_EnableEngine(void);
void Audio_DisableEngine(void);
void Audio_Idle(void);
void Audio_CrashPlayer(int player);
void Audio_Init(void);
void Audio_Quit(void);

void Audio_LoadSample(char *name, int number);
void Audio_LoadMusic(char *name);
void Audio_PlayMusic();
void Audio_StopMusic();
void Audio_SetMusicVolume(float volume);
void Audio_SetFxVolume(float volume);

#ifdef __cplusplus
}
#endif
