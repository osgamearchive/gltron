#include "gltron.h"



int SendPacket( Packet* packet, TCPsocket sock ) {
  void *buf;
  int result;

  buf = malloc(sizeof(packet));
  pos = buf;
  ADD_INT(packet->type);
  ADD_INT(packet->which);
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
    
	   
    
    
      
      
    
