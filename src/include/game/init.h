#ifndef INIT_H
#define INIT_H

extern void initSubsystems(int argc, const char *argv[]);
extern void initScripting(void);
extern void initConfiguration(int argc, const char *argv[]);
extern void initVideo(void);
extern void initAudio(void);
extern void initInput(void);
extern void initGame(void);

/* platform stuff */

extern void videoInit(void);
extern void audioInit(void);
extern void inputInit(void);
#endif
