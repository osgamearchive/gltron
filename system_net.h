#include "system.h"
#include <SDL/SDL_net.h>

void SystemNetExit();
void SystemNetInit();

int SystemNet_Read32(unsigned char *buf);
void SystemNet_Write32(int value, unsigned char *buf);

int SystemTCPrecv(char *buf, int length);
int SystemTCPsend(char *buf, int length);

int SystemCheckSockets();
