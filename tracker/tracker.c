#include "tracker_gltron.h"


static void handle_connection();
static void handle_slot(int which);
static void do_lostconnection(int which);
static void do_server_login( Trackerpacket *packet );
static void do_client_login( Trackerpacket *packet );


void
initNetwork()
{
  int i;
  Net_init();

  //Init the slots
  for(i=0;i<MAX_SLOTS;++i)
    {
      slots[i].active = INACTIVE;   //slots is inactive
      slots[i].sock   = NULL;   //So no sock..
    }
}


void
start_tracker()
{
  //Here we create the two main socket, those which listenning

  //Alocate the sock
  if( Net_allocsocks() )
    {
      fprintf(stderr, "Can't Allocate SocketSet: %s\n", SDLNet_GetError());
      exit(1);
    }

   //Start listening
  if( Net_connect(NULL, settings.port) )
    {
      fprintf(stderr, "Can't Open socket: %s\n", SDLNet_GetError());
      exit(1);
    }

  //Add socket to SocketSet
  Net_addsocket( Net_getmainsock());

  //Server is started!
  printf("server listening to port %d\n", settings.port);
}

static void
do_lostconnection(int which)
{
   Trackerpacket rep;
  int            i;
  

  if( slots[which].active == ACTIVE || slots[which].active == NEGOCIATION )
    {
      slots[which].active=INACTIVE;
	  
      //Say to other...
      rep.which=SERVERID;
      rep.type=TCLOSE;	  
      for(i=0; i< MAX_SLOTS; ++i)
	{
	  if( slots[i].active == ACTIVE && slots[i].type == CLIENT )
	    {	
	      Net_tsendpacket(&rep, slots[i].sock);
	    }
	}
    }
  Net_delsocket(slots[which].sock);
  Net_closesock(slots[which].sock);
  slots[which].sock=NULL;

}


static void
do_server_login( Trackerpacket *packet )
{
  //here we don't need to do lot of things
}

static void
do_client_login( Trackerpacket *packet )
{

}


static void
handle_connection()
{
  TCPsocket   newsock = NULL;
  int         which;
  
  //Accept the new connection
  newsock = SDLNet_TCP_Accept( Net_getmainsock() );
  if( newsock == NULL )
    {
      fprintf(stderr, "Can't Accept connection: %s\n", SDLNet_GetError());
      return;
    }

  //Looking for a free slot
  for(which=0; which < MAX_SLOTS; ++which)
    {
      if( ! slots[which].sock )
	break;
    }
  
  //If which == MAX_SLOTS, there is no free slots...
  if( which >= MAX_SLOTS )
    {
      //We must refuse the connection
      fprintf(stderr, "server is full, refuse login..\n");

      //We close the connection
      Net_closesock(newsock);
    } else {
      //Server is not full...
      //Savings slots
      slots[which].sock   = newsock;
      slots[which].peer   = *SDLNet_TCP_GetPeerAddress(newsock); //Get remote address
      slots[which].active = NEGOCIATION;

      Net_addsocket( slots[which].sock );
      printf("New Connection on slot %d\n", which);
    }
}

static void
handle_slot(int which)
{
  Trackerpacket packet;
  
  //Get the packet...
  if( Net_treceivepacket(&packet, slots[which].sock, which) != 0 )
    {
      //Connection perdu
      do_lostconnection(which);
      return;
    }

  //See if connection is active?
  if( slots[which].active == NEGOCIATION )
    {
      //We need to negociate the slot
      if( packet.type != TLOGIN )
	{
	  fprintf(stderr, "get a packet different from login before negociation\n");
	  slots[which].active=INACTIVE;
	  Net_delsocket(slots[which].sock);
	  Net_closesock(slots[which].sock);
	  slots[which].sock=NULL;
	} else {
	  //that looks ok. See if we accept or not the connection.
	  switch( packet.infos.login.type )
	    {
	    case SERVER:
	      do_server_login(&packet);
	      break;
	    case CLIENT:
	      do_client_login(&packet);
	      break;
	    default:
	      fprintf(stderr, "get a packet different from login before negociation\n");
	      slots[which].active=INACTIVE;
	      Net_delsocket(slots[which].sock);
	      Net_closesock(slots[which].sock);
	      slots[which].sock=NULL;
	      break;
	    }
	}
    }
}

void
handle_tracker()
{
  int   i;

  if( Net_checksocks() )
  {
    printf("has a connection...\n");
    handle_connection();
  }

  //look what is said!
  for(i=0;i<MAX_SLOTS;++i)
    {
      if(slots[i].active != INACTIVE && Net_readysock(slots[i].sock) )
	{
	  handle_slot(i);
	}
    }
}

