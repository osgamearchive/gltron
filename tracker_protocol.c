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
static UDPsocket              udpsock   = NULL;
static UDPpacket             *udppacket = NULL;

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

void
init_ping(int type)
{
  IPaddress ipaddress;
  int       i;

  if( type == 0 )
    {
      /* for( i=PINGPORT; i<PINGPORT+10 && udpsock != NULL; ++i) */
/* 	{ */
/* 	  printf("trying to open udp on port %d\n", i); */
/* 	  udpsock = SDLNet_UDP_Open(i); */
/* 	} */
      udpsock = SDLNet_UDP_Open(PINGPORT);
      if( udpsock==NULL )
	{
	  fprintf(stderr, "can't open udpsock %s\n", SDLNet_GetError());
	  exit(1);
	} else {
	  SDLNet_ResolveHost(&ipaddress, INADDR_ANY, PINGPORT);
	  SDLNet_UDP_Bind(udpsock, 0, &ipaddress);
	}
    } else {
      udpsock = SDLNet_UDP_Open(PINGPORT+10);
      if( udpsock == NULL )
	{
	  fprintf(stderr, "can't open udpsock %s\n", SDLNet_GetError());
	}
    }

  //SDLNet_ResolveHost(&udppacket->address, ipaddress, port);
  if( udpsock == NULL )
    {
      fprintf(stderr, "can't create udp endpoint %s\n", SDLNet_GetError());
    }
  printf("udp endpoint created\n");
  Net_addudpsocket(udpsock);
  //SDLNet_UDP_Bind(udpsock, 0, &ipaddress);
  udppacket = SDLNet_AllocPacket(sizeof(Pingpacket)+1);
}

void
close_ping()
{
  SDLNet_UDP_Close(udpsock);
  Net_deludpsocket(udpsock);
  udpsock = NULL;
  SDLNet_FreePacket(udppacket);
}

void
make_ping(int which, Trackerslots *slots, char *ipaddress, int port)
{
  Pingpacket packet;
  int        i;
  //  IPaddress  address;

  packet.which = which;
  slots[packet.which].ping=0;
  SDLNet_ResolveHost(&udppacket->address, ipaddress, PINGPORT);
  for(i=0; i < NBPINGPACKET; ++i )
    {
      packet.time  = SystemGetElapsedTime();
      printf("sending ping %d to %s:%d\n", i, ipaddress, port);
      packet.num = i;
      udppacket->len=sizeof(Pingpacket);
      memcpy(udppacket->data, &packet, sizeof(Pingpacket));
      
      SDLNet_UDP_Send(udpsock, -1, udppacket);
    }
  
}

void
reply_ping()
{
  int         n;
  
  printf("reply to a ping\n");

  n = SDLNet_UDP_Recv(udpsock, udppacket);
  printf("ping from %s:%hd\n", SDLNet_ResolveIP(&udppacket->address), ntohs(udppacket->address.port));

  SDLNet_UDP_Bind(udpsock, 1, &udppacket->address);
  SDLNet_UDP_Send(udpsock, 1, udppacket);
  SDLNet_UDP_Unbind(udpsock, 1);
}

void
handle_ping(Trackerslots *slots)
{
  Pingpacket  packet;
  int         n;
  

  n = SDLNet_UDP_Recv(udpsock, udppacket);

  memcpy(&packet, udppacket->data, udppacket->len);
  //packet=(Pingpacket)*udppacket->data;
  printf("handle ping for %d\n", packet.which);

  slots[packet.which].ping += SystemGetElapsedTime() -  packet.time;
  slots[packet.which].packets++;
  printf("ping total is %d for %d packets\n", slots[packet.which].ping, slots[packet.which].packets);
}


int
ready_ping()
{
  return ( SDLNet_SocketReady(udpsock) );
}
