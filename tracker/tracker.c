#include "tracker_gltron.h"




static void handle_connection();
static void handle_slot(       int which );
static void do_lostconnection( int which );
static void do_server_login(   int which, Trackerpacket *packet );
static void do_client_login(   int which );
static void do_server_infos(   int which, Trackerpacket *packet );
static void do_notknowninfo(   int which );

void
initNetwork()
{
  int i;

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

void
stop_tracker()
{
  int i;

  //close all connection.
  for(i=0; i< MAX_SLOTS; ++i)
    {
      do_lostconnection(i);
    }

  //clean up connection.
  Net_cleanup();
}

static void
do_lostconnection(int which)
{
  slots[which].active=INACTIVE;
  Net_delsocket(slots[which].sock);
  Net_closesock(slots[which].sock);
  slots[which].sock=NULL;

}

static void
do_notknowninfo(int which)
{
  slots[which].speed=NOTKNOWN;
  slots[which].size=NOTKNOWN;
  slots[which].erase=NOTKNOWN;
  strcpy(slots[which].description, "N/A");
  strcpy(slots[which].version, "N/A");
  //slots[which].ipaddress.host=0;
  slots[which].nbplayers=NOTKNOWN;
}


static void
do_server_login( int which, Trackerpacket *packet )
{
  //here we don't need to do lot of things
  //check passwd
  if( strcmp(packet->infos.login.passwd, settings.passwd) )
    {
      do_lostconnection(which);
      
    } else {
      slots[which].type=SERVER;
      slots[which].active=ACTIVE;

      //making info to N/A
      do_notknowninfo(which);
    }
  printf("a server has logged, waiting for its informations.\n");
}

static void
do_client_login( int which )
{
  int              i, j;
  Trackerpacket    rep;

  //send server list.
  slots[which].type=CLIENT;
  slots[which].active=ACTIVE;

  //prepare packet
  rep.type=TINFOS;
  printf("a client has logging in, sending hime list of servers\n");
  j=0;
  for(i=0; i<MAX_SLOTS; ++i)
    {
      if( slots[i].active==ACTIVE && slots[i].type == SERVER )
	{
	  rep.which=j++;
	  rep.infos.infos.speed = slots[i].speed;
	  rep.infos.infos.size  = slots[i].size;
	  rep.infos.infos.erase = slots[i].erase;
	  strcpy(rep.infos.infos.description, slots[i].description);
	  strcpy(rep.infos.infos.version, slots[i].version);
	  rep.infos.infos.ipaddress= slots[i].ipaddress;
	  rep.infos.infos.nbplayers = slots[i].nbplayers;
	  //Send the packet
	  Net_tsendpacket( &rep , slots[which].sock);
	}
    }
}

static void
do_server_infos(  int which, Trackerpacket *packet )
{
  //here we get the information from a server
  if( slots[which].active != ACTIVE )
    {
      //this is an error, that slot should be active!
      do_lostconnection(which);
      
    } else {
      //saving informations
      slots[which].speed=packet->infos.infos.size;
      slots[which].size=packet->infos.infos.size;
      slots[which].erase=packet->infos.infos.erase;
      strcpy(slots[which].description, packet->infos.infos.description);
      strcpy(slots[which].version, packet->infos.infos.version);
      slots[which].ipaddress.port=packet->infos.infos.ipaddress.port;
      slots[which].nbplayers=packet->infos.infos.nbplayers;

      //updating lastseen
      slots[which].lastseen=SystemGetElapsedTime();
    }
  
  printf("------------------------------------------\n");
  printf("getting server infos %d \n", which);
  printf("speed       : %d\n",slots[which].speed ); 
  printf("size        : %d\n",slots[which].size ); 
  printf("erase       : %d\n",slots[which].erase ); 
  printf("description : %s\n",slots[which].description ); 
  printf("version     : %s\n",slots[which].version );  

  printf("IP          : %d.%d.%d.%d:%i\n",
	 (SDLNet_Read32(&slots[which].ipaddress.host) & 0xff000000) >> 24,
	  (SDLNet_Read32(&slots[which].ipaddress.host) & 0x00ff0000) >> 16,
	 (SDLNet_Read32(&slots[which].ipaddress.host) & 0x0000ff00) >> 8,
	 SDLNet_Read32(&slots[which].ipaddress.host) & 0x000000ff,
	 slots[which].ipaddress.port
 ); 
  printf("nbplayers   : %d\n",slots[which].nbplayers ); 
  printf("------------------------------------------\n");

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
      slots[which].sock        = newsock;
      slots[which].ipaddress   = *SDLNet_TCP_GetPeerAddress(newsock); //Get remote address
      slots[which].active      = NEGOCIATION;
      slots[which].lastseen    = SystemGetElapsedTime();

      Net_addsocket( slots[which].sock );
      printf("New Connection on slot %d\n", which);
    }
}

static void
handle_slot(int which)
{
  Trackerpacket packet;
  
  //Get the packet...
  if( Net_treceivepacket(&packet, slots[which].sock) != 0 )
    {
      //Connection perdu
      do_lostconnection(which);
      return;
    }

  printf("got a packet type %d from %d\n", packet.type, packet.which);
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
	      do_server_login(which, &packet);
	      break;
	    case CLIENT:
	      do_client_login(which);
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
      return;
    }

  switch( packet.type )
    {
    case TINFOS:
      do_server_infos(which, &packet);
      break;
    default:
      fprintf(stderr, "error unknown packet type:%d\n", packet.type);
      break;
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

void
check_serveractivity()
{
  int i;

  for(i=0; i< MAX_SLOTS; ++i)
    {
      if( slots[i].active != INACTIVE && slots[i].type == SERVER )
	{
	  if( ( SystemGetElapsedTime()- slots[i].lastseen ) > settings.timeout*1000 )
	    {
	      printf("server %d answer is timeout! %d\n", i, SystemGetElapsedTime() - slots[i].lastseen );
	      //that server is down now!
	      do_lostconnection(i);
	      do_notknowninfo(i);
	    }
	}
    }
}
