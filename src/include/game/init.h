#ifndef INIT_H
#define INIT_H

extern void initSubsystems(int argc, const char *argv[]);
extern void initFilesystem();
extern void initScripting();
extern void initConfiguration(int argc, const char *argv[]);
extern void initVideo();
extern void initAudio();
extern void initInput();
extern void initGame();

/* platform stuff */

extern void videoInit();
extern void audioInit();
extern void inputInit();
#endif
