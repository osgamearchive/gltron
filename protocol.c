#ifdef __NETWORK__
//this is the client
#include "gltron.h"
static int nbsocks = 2;
#else
//this is the server
#include "server_gltron.h"
static int nbsocks = MAX_PLAYERS+1;
#endif


static SDLNet_SocketSet       socketset = NULL;
static TCPsocket              tcpsock   = NULL;
int isConnected = 0;
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
Net_sendpacket( Packet  *packet , TCPsocket sock )
{
  if( packet == NULL )
    {
      return cantsendpacket;
    }
  printf("sending packet size: %d\n", sizeof(Packet));
  if( ( SDLNet_TCP_Send(sock, (void *)packet, sizeof(Packet))) <= 0 )
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
