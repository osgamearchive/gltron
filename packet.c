#include "gltron.h"

void *pos;
#define ADD_STRING(x) memcpy(pos, x, sizeof(x)); pos += sizeof(x)
#define ADD_INT(x) SDLNet_Write32(x, pos); pos += 4

int SendPacket( Packet* packet, TCPsocket sock ) {
  void *buf;
  int result;

  buf = malloc(sizeof(packet));
  pos = buf;
  ADD_INT(packet->type);
  switch(packet->type) {
  case LOGIN:
    ADD_STRING(packet->infos.login.version);
    ADD_STRING(packet->infos.login.nick);
    break;
  case LOGINREP:
    ADD_INT(packet->infos.loginrep.accept);
    ADD_STRING(packet->infos.loginrep.message);
    break;
  }
  result = SDLNET_TCP_Send(sock, buf, pos - buf);
  free(buf);
  return result;
}
    
	   
    
    
      
      
    
