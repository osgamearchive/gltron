#ifdef __NETWORK__
//this is the client
#include "gltron.h"
static int nbsocks = 1;
#else
//this is the server
#include "server/server_gltron.h"
static int nbsocks = MAX_PLAYERS+1;
#endif
int netrulenbwins = 5;
int netruletime   = 0;

static SDLNet_SocketSet       socketset = NULL;
static TCPsocket              tcpsock   = NULL;
int isConnected = 0;


static char *pos;

#define ADD_STRING(x)     memcpy(pos, x, sizeof(x)); pos += sizeof(x)
#define GET_STRING(x, y)  memcpy(x, y, sizeof(x));   y   += sizeof(x)
#define ADD_INT16(x)      SDLNet_Write16(x, pos);    pos += 2
#define ADD_INT32(x)      SDLNet_Write32(x, pos);    pos += 4
#define GET_INT16(x, y)   x=SDLNet_Read16(y);        y   += 2
#define GET_INT32(x, y)   x=SDLNet_Read32(y);        y   += 4


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
   if( sock != NULL && SDLNet_SocketReady(sock) )
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
Net_preparepacket(Packet* packet, char *buf)
{
  int i;

  if( buf == NULL )
    return 0;
  
  pos = buf;
  ADD_INT16(packet->type);
  ADD_INT16(packet->which);
  printf("send packet type %d from %d\n", packet->type, packet->which);
  switch(packet->type) {
  case LOGIN:
    ADD_STRING(packet->infos.login.version);
    ADD_STRING(packet->infos.login.nick);
    break;
  case LOGINREP:
    ADD_INT16(packet->infos.loginrep.accept);
    ADD_STRING(packet->infos.loginrep.message);
    break;
  case USERINFO:
    ADD_INT16(packet->infos.userinfo.which);
    ADD_INT16(packet->infos.userinfo.ismaster);
    ADD_INT16(packet->infos.userinfo.color);
    ADD_STRING(packet->infos.userinfo.nick);
    break;
  case SERVERINFO:
    ADD_INT16(packet->infos.serverinfo.serverstate);
    ADD_INT16(packet->infos.serverinfo.players);
    break;
  case CHAT:
    ADD_STRING(packet->infos.chat.mesg);
    ADD_INT16(packet->infos.chat.which);
    break;
  case GAMERULES:
    ADD_INT16(packet->infos.gamerules.players);
    ADD_INT16(packet->infos.gamerules.speed*1000);
    ADD_INT16(packet->infos.gamerules.eraseCrashed);
    //TODO: need do be finished: how to add Time, float, and int * for
    //Time
    ADD_INT16(packet->infos.gamerules.time.current);
    ADD_INT16(packet->infos.gamerules.time.lastFrame);
    ADD_INT16(packet->infos.gamerules.time. offset);
    ADD_INT16(packet->infos.gamerules.time. dt);

    ADD_INT16(packet->infos.gamerules.gamespeed);
    ADD_INT16(packet->infos.gamerules.grid_size);
    ADD_INT16(packet->infos.gamerules.arena_size);
    break;
  case STARTPOS:
    //startPos
    for(i=0; i< MAX_PLAYERS*3; ++i)
      {
	ADD_INT16(packet->infos.startpos.startPos[i]);
      }
    break;
  case NETRULES:
    ADD_INT16(packet->infos.netrules.nbWins);
    ADD_INT16(packet->infos.netrules.time);   
    break;
  case GAMESET:
    ADD_INT16(packet->infos.gameset.eraseCrashed);
    ADD_INT16(packet->infos.gameset.gamespeed);
    ADD_INT16(packet->infos.gameset.arena_size);
    break;
  case SCORE:
    ADD_INT16(packet->infos.score.winner);
    //Points for each players
    for(i=0; i< MAX_PLAYERS; ++i)
      {
	ADD_INT16(packet->infos.score.points[i]);
      }
    break;
  case SNAPSHOT:
    //TODO: how send events[] : see later... ( easy )
    break;
  case EVENT:
    ADD_INT16(packet->infos.event.event.type);
    ADD_INT16(packet->infos.event.event.player);
    ADD_INT16(packet->infos.event.event.x);
    ADD_INT16(packet->infos.event.event.y);
    ADD_INT16(packet->infos.event.event.timestamp);
    break;
  case PLAYERSPING:
    for(i=0; i< MAX_PLAYERS; ++i)
      {
	ADD_INT16(packet->infos.playersping.ping[i]);
      }
    break;
  case ACTION:
    ADD_INT16(packet->infos.action.type);
    ADD_INT16(packet->infos.action.which);   
    break;
  }
  return pos - buf;
}

int
Net_sendpacket( Packet  *packet , TCPsocket sock )
{
  char *buff;
  int  len;

  if( packet == NULL )
    {
      printf("packet is NULL\n");
      return cantsendpacket;
    }

  buff = malloc(PACKETSIZE);  //Allocate max packet size...

  //printf("packet size %d ( float is %d )\n", sizeof(Packet), sizeof(float));

  //First : we send the header
  SDLNet_Write16(packet->type, buff);
  printf("sending header: %d at %d\n", packet->type, game2->time.current);
  if( ( SDLNet_TCP_Send(sock, buff, sizeof(Sint16))) < sizeof(Sint16) )
    {
      free(buff);
      buff=NULL;
      printf("error while sending a header!!\n");
      return connectionclosed;
    }

  //Parse packet to send
  len =  Net_preparepacket(packet, buff);
  printf("sending packet size: %d\n", len);
  printf("type %d from %d\n", packet->type, packet->which);

  //Send the packet itself...
  if( ( SDLNet_TCP_Send(sock, buff, len)) < len )
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
Net_handlepacket(Packet* packet, char *buf)
{
  int i;

  GET_INT16(packet->type, buf);
  GET_INT16(packet->which, buf);
  printf("get packet type %hd from %hd\n", packet->type, packet->which);
  switch(packet->type) {
  case LOGIN:
    GET_STRING(packet->infos.login.version, buf);
    GET_STRING(packet->infos.login.nick, buf);
    break;
  case LOGINREP:
    GET_INT16(packet->infos.loginrep.accept, buf);
    GET_STRING(packet->infos.loginrep.message, buf);
    break;
  case USERINFO:
    GET_INT16(packet->infos.userinfo.which, buf);
    GET_INT16(packet->infos.userinfo.ismaster, buf);
    GET_INT16(packet->infos.userinfo.color, buf);
    GET_STRING(packet->infos.userinfo.nick, buf);
    break;
  case SERVERINFO:
    GET_INT16(packet->infos.serverinfo.serverstate, buf);
    GET_INT16(packet->infos.serverinfo.players, buf);
    break;
  case CHAT:
    GET_STRING(packet->infos.chat.mesg, buf);
    GET_INT16(packet->infos.chat.which, buf);
    break;
  case GAMERULES:
    GET_INT16(packet->infos.gamerules.players, buf);
    GET_INT16(packet->infos.gamerules.speed, buf);
    packet->infos.gamerules.speed/=1000;
    GET_INT16(packet->infos.gamerules.eraseCrashed, buf);
    //time...
    GET_INT16(packet->infos.gamerules.time.current, buf);
    GET_INT16(packet->infos.gamerules.time.lastFrame, buf);
    GET_INT16(packet->infos.gamerules.time. offset, buf);
    GET_INT16(packet->infos.gamerules.time. dt, buf);

    GET_INT16(packet->infos.gamerules.gamespeed, buf);
    GET_INT16(packet->infos.gamerules.grid_size, buf);
    GET_INT16(packet->infos.gamerules.arena_size, buf);
    break;
  case STARTPOS:   
    //startPos
    for(i=0; i< MAX_PLAYERS*3; ++i)
      {
	GET_INT16(packet->infos.startpos.startPos[i], buf);
      }
    break;
  case NETRULES:
    GET_INT16(packet->infos.netrules.nbWins, buf);
    GET_INT16(packet->infos.netrules.time, buf);
    break;
  case GAMESET:
    GET_INT16(packet->infos.gameset.eraseCrashed, buf);
    GET_INT16(packet->infos.gameset.gamespeed, buf);
    GET_INT16(packet->infos.gameset.arena_size, buf);
    break;
  case SCORE:
    GET_INT16(packet->infos.score.winner, buf);
    for(i=0; i< MAX_PLAYERS; ++i)
      {
	GET_INT16(packet->infos.score.points[i], buf);
      }
    break;
  case SNAPSHOT:
    //TODO:
    break;
  case EVENT:
    GET_INT16(packet->infos.event.event.type, buf);
    GET_INT16(packet->infos.event.event.player, buf);
    GET_INT16(packet->infos.event.event.x, buf);
    GET_INT16(packet->infos.event.event.y, buf);
    GET_INT16(packet->infos.event.event.timestamp, buf);
    break;
  case PLAYERSPING:
    for(i=0; i< MAX_PLAYERS; ++i)
      {
	GET_INT16(packet->infos.playersping.ping[i], buf);
      }  
    break;
  case ACTION:
    GET_INT16(packet->infos.action.type, buf);
    GET_INT16(packet->infos.action.which, buf);   
    break;
  }
}

int
get_packetsize( int type )
{
  switch(type) {
  case LOGIN:
    return 2 * sizeof(Sint16) + 2 * 9 * sizeof(char);
    break;
  case LOGINREP:
    return 3 * sizeof(Sint16) + 32 * sizeof(char);
    break;
  case USERINFO:
    return 5 * sizeof(Sint16) + 9 * sizeof(char);
    break;
  case SERVERINFO:
    return 4 * sizeof(Sint16);
    break;
  case CHAT:
    return 3 * sizeof(Sint16) + 32 * sizeof(char);
    break;
  case GAMERULES:
    return 12 * sizeof(Sint16); 
    break;
  case STARTPOS:
    return 2 * sizeof(Sint16) + MAX_PLAYERS *  3 * sizeof(Sint16);
    break;
  case NETRULES:
    return 4 * sizeof(Sint16);
    break;
  case GAMESET:
    return 5 * sizeof(Sint16);
    break;
  case SCORE:
    return 3 * sizeof(Sint16) + MAX_PLAYERS * sizeof(Sint16);
    break;
  case SNAPSHOT:
    return 2 * sizeof(Sint16);
    break;
  case EVENT:
    return 7 * sizeof(Sint16);
    break;
  case PLAYERSPING:
    return (2 + MAX_PLAYERS)* sizeof(Sint16);
    break;
  case ACTION:
    return 4 * sizeof(Sint16);
    break;
  }
  return 0;
}


int
Net_receivepacket( Packet *packet, TCPsocket sock, int which, int type )
{
  int   rLen, len;
  char *buff;

  if( packet == NULL )
    {
      return cantgetpacket;
    }
  printf("getting packet for %d, type %d at %d\n", which, slots[which].packet, game2->time.current);
  if( type == HEADER )
    {
      buff =  malloc(sizeof(Sint16));
      rLen = SDLNet_TCP_Recv(sock, (void *) buff, sizeof(Sint16));
      if( rLen < sizeof(Sint16) )
	{
	  free(buff);
	  buff=NULL;
	  return connectionclosed;
	}
      //parse
      slots[which].packet=SDLNet_Read16(buff);
      
      printf("get a header, next packet for %d is %d\n", which, slots[which].packet);
    } else {

      //Type of packet is type! so we know len of the packet
      len = get_packetsize(type);

      buff =  malloc(len);


      rLen = SDLNet_TCP_Recv(sock, (void *) buff, len);
      
      if( rLen < len )
	{
	  free(buff);
	  buff=NULL;
	  return connectionclosed;
	}
      
      //Only do the work if necessary
      Net_handlepacket(packet, buff);
      slots[which].packet=HEADER;
      printf("get a packet size is %d\n", len);
    }



  free(buff);
  buff=NULL;
  return 0;
}
