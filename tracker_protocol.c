#ifdef TRACKER
#include "tracker/tracker_gltron.h"
#else
#ifndef __NETWORK__
#include "server/server_gltron.h"
#else
#include "gltron.h"
#endif
#endif

//Here u use UDP socks

static TCPsocket              tcpsock   = NULL;

#define ADD_STRING(x)     memcpy(pos, x, sizeof(x)); pos += sizeof(x)
#define GET_STRING(x, y)  memcpy(x, y, sizeof(x));   y   += sizeof(x)
#define ADD_INT16(x)      SDLNet_Write16(x, pos);    pos += 2
#define ADD_INT32(x)      SDLNet_Write32(x, pos);    pos += 4
#define GET_INT16(x, y)   x=SDLNet_Read16(y);        y   += 2
#define GET_INT32(x, y)   x=SDLNet_Read32(y);        y   += 4

static char *pos;

static int  Net_tpreparepacket(Trackerpacket* packet, char *buf);
static void Net_thandlepacket(Trackerpacket* packet, char *buf);

static void Net_thandlepacket(Trackerpacket* packet, char *buf)
{

  GET_INT16(packet->type, buf);
  GET_INT16(packet->which, buf);
  printf("get packet type %hd from %hd\n", packet->type, packet->which);
  switch(packet->type) {
  case TLOGIN:
    GET_INT16(packet->infos.login.type, buf);
    GET_STRING(packet->infos.login.passwd, buf);
    break;
  case TINFOS:
    GET_INT16(packet->infos.infos.speed, buf);
    GET_INT16(packet->infos.infos.size, buf);
    GET_INT16(packet->infos.infos.erase, buf);
    GET_STRING(packet->infos.infos.description, buf);
    GET_STRING(packet->infos.infos.version, buf);
    GET_INT32(packet->infos.infos.ipaddress.host, buf);
    GET_INT16(packet->infos.infos.ipaddress.port, buf);
    GET_INT16(packet->infos.infos.nbplayers, buf);
    break;
  }
}

static int
Net_tpreparepacket(Trackerpacket* packet, char *buf)
{

  if( buf == NULL )
    return 0;
  
  pos = buf;
  ADD_INT16(packet->type);
  ADD_INT16(packet->which);
  printf("send packet type %d from %d\n", packet->type, packet->which);
  switch(packet->type) {
  case TLOGIN:
    ADD_INT16(packet->infos.login.type);
    ADD_STRING(packet->infos.login.passwd);
    break;
  case TINFOS:
    ADD_INT16(packet->infos.infos.speed);
    ADD_INT16(packet->infos.infos.size);
    ADD_INT16(packet->infos.infos.erase);
    ADD_STRING(packet->infos.infos.description);
    ADD_STRING(packet->infos.infos.version);
    ADD_INT32(packet->infos.infos.ipaddress.host);
    ADD_INT16(packet->infos.infos.ipaddress.port);
    ADD_INT16(packet->infos.infos.nbplayers);
    break;
  }
  return pos - buf;
}

int
Net_treceivepacket(Trackerpacket *packet , TCPsocket sock)
{
 int   rLen, len;
 char *buff;

 len = TPACKETSIZE;

 buff =  malloc(len);

  rLen = SDLNet_TCP_Recv(sock, (void *) buff, len);
      
  if( rLen < len )
    {
      free(buff);
      buff=NULL;
      return connectionclosed;
    }
  
  //Only do the work if necessary
  Net_thandlepacket(packet, buff);
  free(buff);
  buff = 0;
  
  return 0;
}

int
Net_tsendpacket (Trackerpacket *packet, TCPsocket sock)
{
  char *buff;
  int  len;

  if( packet == NULL )
    {
      printf("packet is NULL\n");
      return cantsendpacket;
    }

  buff = malloc(TPACKETSIZE);  //Allocate max packet size...

  //Parse packet to send
  len =  Net_tpreparepacket(packet, buff);
  printf("sending packet size: %d\n", len);
  printf("type %d from %d\n", packet->type, packet->which);

  //Send the packet itself...
  if( ( SDLNet_TCP_Send(sock, buff, TPACKETSIZE)) < TPACKETSIZE )
    {
      free(buff);
      buff=NULL;
      return connectionclosed;
    }
  free(buff);
  buff=NULL;
  return 0;
}

//only used by server and client.
int
Net_tconnect( char *server, int port)
{
  IPaddress    serverIP;

  //Getting IPaddress
  SDLNet_ResolveHost(&serverIP, server, port);

  if(server == NULL )
    {
      return cantconnect;
    } else {
      //This is the client...
      if( serverIP.host == INADDR_NONE )
	{
	  return cantfindhost;
	} else {
	  //Open connection
	  tcpsock = SDLNet_TCP_Open(&serverIP);
	  if( tcpsock == NULL )
	    {
	      return cantconnect;
	    } else {
	      printf("connected to tracker...\n");
	      return 0;
	    }
	}
    }
  return unknownerror;
}


void
Net_tdisconnect( )
{
  if( tcpsock != NULL )
    {
      //SDLNet_TCP_DelSocket(socketset, tcpsock);
      SDLNet_TCP_Close(tcpsock);
      tcpsock = NULL;
    }
}

TCPsocket
Net_gettrackersock()
{
  return tcpsock;
}
