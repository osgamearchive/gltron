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
#define GET_STRING(x, y) memcpy(x, y, sizeof(x)); y += sizeof(x)
#define GET_INT(x, y) x=SDLNet_Read32(y); y += 4


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
  int i;

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
    ADD_STRING(packet->infos.chat.mesg);
    ADD_INT(packet->infos.chat.which);
    break;
  case GAMERULES:
    ADD_INT(packet->infos.gamerules.players);
    ADD_INT(packet->infos.gamerules.speed*1000);
    ADD_INT(packet->infos.gamerules.eraseCrashed);
    //TODO: need do be finished: how to add Time, float, and int * for
    //startPos
    for(i=0; i< MAX_PLAYERS*3; ++i)
      {
	ADD_INT(packet->infos.gamerules.startPos[i]);
      }
    //Time
    ADD_INT(packet->infos.gamerules.time.current);
    ADD_INT(packet->infos.gamerules.time.lastFrame);
    ADD_INT(packet->infos.gamerules.time. offset);
    ADD_INT(packet->infos.gamerules.time. dt);

    ADD_INT(packet->infos.gamerules.gamespeed);
    ADD_INT(packet->infos.gamerules.grid_size);
    ADD_INT(packet->infos.gamerules.arena_size);
    break;
  case NETRULES:
    ADD_INT(packet->infos.netrules.nbWins);
    ADD_INT(packet->infos.netrules.time);   
    break;
  case SCORE:
    ADD_INT(packet->infos.score.winner);
    //Points for each players
    for(i=0; i< MAX_PLAYERS; ++i)
      {
	ADD_INT(packet->infos.score.points[i]);
      }
    break;
  case SNAPSHOT:
    //TODO: how send events[] : see later... ( easy )
    break;
  case EVENT:
    ADD_INT(packet->infos.event.event.type);
    ADD_INT(packet->infos.event.event.player);
    ADD_INT(packet->infos.event.event.x);
    ADD_INT(packet->infos.event.event.y);
    ADD_INT(packet->infos.event.event.timestamp);
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
  void *buff;
  int  len;

  if( packet == NULL )
    {
      return cantsendpacket;
    }

  buff = malloc(PACKETSIZE);

  printf("packet size %d ( float is %d )\n", sizeof(Packet), sizeof(float));

  len =  Net_preparepacket(packet, buff);
  printf("sending packet size: %d\n", len);
  printf("type %d from %d\n", packet->type, packet->which);
  //Packet mus be the same size...
  if( ( SDLNet_TCP_Send(sock, buff, PACKETSIZE)) < PACKETSIZE )
    {
      free(buff);
      buff=NULL;
      return connectionclosed;
    }
  free(buff);
  buff=NULL;
  return 0;
}


/** Important: get things in the same order as they 've been send! */
void
Net_handlepacket(Packet* packet, void *buf)
{
  int i;

  GET_INT(packet->type, buf);
  GET_INT(packet->which, buf);
  printf("get packet type %d from %d\n", packet->type, packet->which);
  switch(packet->type) {
  case LOGIN:
    GET_STRING(packet->infos.login.version, buf);
    GET_STRING(packet->infos.login.nick, buf);
    break;
  case LOGINREP:
    GET_INT(packet->infos.loginrep.accept, buf);
    GET_STRING(packet->infos.loginrep.message, buf);
    break;
  case USERINFO:
    GET_INT(packet->infos.userinfo.which, buf);
    GET_INT(packet->infos.userinfo.ismaster, buf);
    GET_INT(packet->infos.userinfo.color, buf);
    GET_STRING(packet->infos.userinfo.nick, buf);
    break;
  case SERVERINFO:
    GET_INT(packet->infos.serverinfo.serverstate, buf);
    GET_INT(packet->infos.serverinfo.players, buf);
    break;
  case CHAT:
    GET_STRING(packet->infos.chat.mesg, buf);
    GET_INT(packet->infos.chat.which, buf);
    break;
  case GAMERULES:
    GET_INT(packet->infos.gamerules.players, buf);
    GET_INT(packet->infos.gamerules.speed, buf);
    packet->infos.gamerules.speed/=1000;
    GET_INT(packet->infos.gamerules.eraseCrashed, buf);
    //startPos
    for(i=0; i< MAX_PLAYERS*3; ++i)
      {
	GET_INT(packet->infos.gamerules.startPos[i], buf);
      }
    //time...
    GET_INT(packet->infos.gamerules.time.current, buf);
    GET_INT(packet->infos.gamerules.time.lastFrame, buf);
    GET_INT(packet->infos.gamerules.time. offset, buf);
    GET_INT(packet->infos.gamerules.time. dt, buf);

    GET_INT(packet->infos.gamerules.gamespeed, buf);
    GET_INT(packet->infos.gamerules.grid_size, buf);
    GET_INT(packet->infos.gamerules.arena_size, buf);
    break;
  case NETRULES:
    GET_INT(packet->infos.netrules.nbWins, buf);
    GET_INT(packet->infos.netrules.time, buf);
    break;
  case SCORE:
    GET_INT(packet->infos.score.winner, buf);
    for(i=0; i< MAX_PLAYERS; ++i)
      {
	GET_INT(packet->infos.score.points[i], buf);
      }
    break;
  case SNAPSHOT:
    //TODO:
    break;
  case EVENT:
    GET_INT(packet->infos.event.event.type, buf);
    GET_INT(packet->infos.event.event.player, buf);
    GET_INT(packet->infos.event.event.x, buf);
    GET_INT(packet->infos.event.event.y, buf);
    GET_INT(packet->infos.event.event.timestamp, buf);
    break;
  case ACTION:
    GET_INT(packet->infos.action.type, buf);
    GET_INT(packet->infos.action.which, buf);   
    break;
  }
}


int
Net_receivepacket( Packet *packet, TCPsocket sock )
{
  int   rLen;
  void *buff;

  if( packet == NULL )
    {
      return cantgetpacket;
    }
  buff =  malloc(PACKETSIZE);


  rLen = SDLNet_TCP_Recv(sock, (void *) buff, PACKETSIZE);

  if( rLen < PACKETSIZE )
    {
      free(buff);
      buff=NULL;
      return connectionclosed;
    }

  //Only do the work if necessary
  Net_handlepacket(packet, buff);

  free(buff);
  buff=NULL;
  return 0;
}
