#ifdef __NETWORK__
//this is the client
#include "gltron.h"
static int nbsocks = 2;
#else
//this is the server
#include "server_gltron.h"
static int nbsocks = MAX_PLAYERS+1;
#endif
int netrulenbwins = 5;
int netruletime  = -1;

static SDLNet_SocketSet       socketset = NULL;
static TCPsocket              tcpsock   = NULL;
int isConnected = 0;


static void *pos;
#define ADD_STRING(x) memcpy(pos, x, sizeof(x)); pos += sizeof(x)
#define ADD_INT(x) SDLNet_Write32(x, pos); pos += 4


/** Core functions */
void
Net_init( )
{
  if( SDLNet_Init() < 0 )
    {
      fprintf(stderr, "Can't init SDLNet : %s\n", SDLNet_GetError());
      exit(1);
    }  
}

void
Net_cleanup( )
{
  if( tcpsock != NULL )
    {
      SDLNet_TCP_Close(tcpsock);
      tcpsock=NULL;
    }

  if( socketset != NULL )
    {
      SDLNet_FreeSocketSet(socketset);
      socketset = NULL;
    }
}

int
Net_connect( char *server, int port)
{
  IPaddress    serverIP;

  //Getting IPaddress
  SDLNet_ResolveHost(&serverIP, server, port);

  if(server == NULL )
    {
      //This is the server...
    
      //Start listening to port.
      tcpsock = SDLNet_TCP_Open(&serverIP);
      if( tcpsock == NULL )
	{
	  Net_cleanup();
	  fprintf(stderr, "Can't Open socket: %s\n", SDLNet_GetError());
	  exit(1);
	}
      isConnected=1;
      return 0;
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
	      printf("connected...\n");
	      isConnected = 1;
	      return 0;
	    }
	}
    }
  return unknownerror;
}

void
Net_disconnect( )
{
  if( tcpsock != NULL )
    {
      SDLNet_TCP_DelSocket(socketset, tcpsock);
      SDLNet_TCP_Close(tcpsock);
      tcpsock = NULL;
    }
  isConnected = 0;
}


void
Net_closesock( TCPsocket sock)
{
  if( sock != NULL )
    {
      SDLNet_TCP_Close(sock);
      sock = NULL;
    }
}

int
Net_allocsocks( )
{
  socketset = SDLNet_AllocSocketSet(nbsocks);
  if( socketset == NULL )
    {
      return cantallocsockset;
    }
  SDLNet_TCP_AddSocket(socketset, tcpsock);
  return 0;
}

int
Net_addsocket(TCPsocket sock)
{
  SDLNet_TCP_AddSocket(socketset, sock);
  return 0;
}

int
Net_delsocket(TCPsocket sock)
{
  SDLNet_TCP_DelSocket(socketset, sock);
  return 0;
}


int
Net_readysock( TCPsocket sock )
{
   if( SDLNet_SocketReady(sock) )
    {
      return 1;
    }
   return 0;
}

int
Net_checksocks( )
{  
  SDLNet_CheckSockets(socketset, 0);
  
  // something appens, look what it was...
  if( SDLNet_SocketReady(tcpsock) )
    {
      return 1;
    }
  
  return 0; 
}

TCPsocket
Net_getmainsock( )
{
  return tcpsock;
}


int
Net_preparepacket(Packet* packet, void *buf)
{
  if( buf == NULL )
    return 0;
  
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
  case USERINFO:
    ADD_INT(packet->infos.userinfo.which);
    ADD_INT(packet->infos.userinfo.ismaster);
    ADD_INT(packet->infos.userinfo.color);
    ADD_STRING(packet->infos.userinfo.nick);
    break;
  case SERVERINFO:
    ADD_INT(packet->infos.serverinfo.serverstate);
    ADD_INT(packet->infos.serverinfo.players);
    break;
  case CHAT:
    ADD_INT(packet->infos.chat.which);
    ADD_STRING(packet->infos.chat.mesg);
    break;
  case GAMERULES:
    ADD_INT(packet->infos.gamerules.players);
    //ADD_INT(packet->infos.gamerules.speed*1000);
    ADD_INT(packet->infos.gamerules.eraseCrashed);
    //TODO: nee do be finished: how to add Time, float, and int * for
    //startPos
    break;
  case NETRULES:
    ADD_INT(packet->infos.netrules.nbWins);
    ADD_INT(packet->infos.netrules.time);   
    break;
  case SCORE:
    ADD_INT(packet->infos.score.winner);
    //TODO: same int* for points ?
    break;
  case SNAPSHOT:
    //TODO: how send events[]
    break;
  case EVENT:
    //TODO: how send an event?
    break;
  case ACTION:
    ADD_INT(packet->infos.action.type);
    ADD_INT(packet->infos.action.which);   
    
    break;
  }
  return pos - buf;
}

int
Net_sendpacket( Packet  *packet , TCPsocket sock )
{
  void *buff = malloc(sizeof(packet));
  int  len = Net_preparepacket(packet, buff);

  if( packet == NULL )
    {
      return cantsendpacket;
    }

  

  printf("sending packet size: %d\n", len);
  if( ( SDLNet_TCP_Send(sock, buff, len) <= 0 ))
    {
      return connectionclosed;
    }
  return 0;
}

int
Net_receivepacket( Packet *packet, TCPsocket sock )
{
  int rLen;

  if( packet == NULL )
    {
      return cantgetpacket;
    }
  rLen = SDLNet_TCP_Recv(sock, (void *) packet, sizeof(Packet));
  printf("get packet size: %d\n", sizeof(Packet));
  if( rLen <= 0 )
    {
      return connectionclosed;
    }
  return 0;
}
