#include "system_net_sdl.h"

static TCPsocket socket; /* that's a pointer! */

#define SERVER_HOST "tulpe.ards.dataway.ch"
#define SERVER_PORT 23460

void SystemNetExit() {
  SDLNet_Quit();
}

void SystemNetInit() {
  SDLNet_Init();
}

void SystemNet_Write32(int value, unsigned char *buf) {
  SDLNet_Write32(value, buf);
}

int SystemNet_Read32(unsigned char *buf) {
  return SDLNet_Read32(buf);
}

int SystemConnect() {
  IPaddress address;


  if(SDLNet_ResolveHost(&address, SERVER_HOST, SERVER_PORT) == 0) {
    fprintf(stderr, "couldn't resolve host %s\n", SERVER_HOST);
    return 1;
  }

  if((socket = SDLNet_TCP_Open(&address)) == NULL) {
    fprintf(stderr, "coulnd't establish socket connection\n");
    return 1;
  }
  return 0;
}

int SystemTCPrecv(char *buf, int length) {
  return SDLNet_TCP_Recv(socket, buf, length);
}

int SystemTCPsend(char *buf, int length) {
  return SDLNet_TCP_Send(socket, buf, length);
}

int SystemCheckSockets() {
  return 0;
}



