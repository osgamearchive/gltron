#include "server_gltron.h"

void
start_server()
{
  int        i;


  //Init the slots
  for(i=0;i<MAX_PLAYERS;++i)
    {
      slots[i].active = 0;   //slots is inactive
      slots[i].sock   = 0;   //So no sock..
    }

  //Alocate the sock
  if( Net_allocsocks() )
    {
      fprintf(stderr, "Can't Allocate SocketSet: %s\n", SDLNet_GetError());
      exit(1);
    }

  //Start listening
  if( Net_connect(NULL, server_port) )
    {
      fprintf(stderr, "Can't Open socket: %s\n", SDLNet_GetError());
      exit(1);
    }

  //Add socket to SocketSet
  Net_addsocket( Net_getmainsock());
  
  sState   = waitState;
  nbUsers  = 0;

  //Server is started!
  printf("server listening to port %d\n", server_port);
}

void
initNetwork()
{
 
  Net_init();

  //Put all ai_players to 'none'
  game->settings->ai_player1=2;
  game->settings->ai_player2=2;
  game->settings->ai_player3=2;
  game->settings->ai_player4=2;

  game2->mode = GAME_NETWORK_RECORD;
  sState = waitState;
}

void
stop_server()
{
  Net_cleanup();
}

void
do_lostplayer(int which )
{
  Packet rep;
  int    i;
  

  if( slots[which].active == 1 )
    {
      slots[which].active=0;
      nbUsers--;

      //Say to other...
      rep.which=SERVERID;
      rep.type=ACTION;
      rep.infos.action.type=PART;
      rep.infos.action.which=which;

      printf("%s nous quitte.\n", slots[which].name);

      //TODO: If it was the game master, change that

      for(i=0; i< 4; ++i)
	{
	  if( slots[i].active == 1 )
	    {	 
	      Net_sendpacket(&rep, slots[i].sock);
	    }
	}
      //TODO: if there is no1 left, init game to default
      //and go to waitState.

    }
  Net_delsocket(slots[which].sock);
  Net_closesock(slots[which].sock);
  slots[which].sock=NULL;
}

int
check_version(char *vers )
{
  //TODO: check version.
  return 1;
}

void
do_login( int which, Packet packet )
{
  Packet rep, rep2;
  int    i;

  if( ! check_version(packet.infos.login.version) )
    {
      printf("version mauvaise: %s\n", packet.infos.login.version);
      //Send the reponse deny...
      rep.which  = SERVERID;
      rep.type   = LOGINREP;
      rep.infos.loginrep.accept = 0;
      strcpy(rep.infos.loginrep.message, "La version de gltron est incompatible.\n");
      Net_sendpacket(&rep, slots[which].sock);
      Net_closesock(slots[which].sock);
      Net_delsocket(slots[which].sock);
      slots[which].active=0;
      slots[which].sock=NULL;
      return;
    }
  //Look if it is a game master
  if( nbUsers == 0 )
    {
      slots[which].isMaster = 1;
    } else {
      slots[which].isMaster = 0;
    }

  //TODO Search for the game master...

  //Accept the connection
  nbUsers++;

  rep.which  = which;
  rep.type   = LOGINREP;
  rep.infos.loginrep.accept = 1;
  strcpy(rep.infos.loginrep.message, "Welcome to gltron network.\n");
  printf("+ sending loginrep\n");
  Net_sendpacket(&rep, slots[which].sock);
  printf("- sending loginrep\n");

  //Changing server state...
  sState = preGameState;
  
  //Send server infos...
  rep.which   = SERVERID;
  rep.type    = SERVERINFO;
  rep.infos.serverinfo.serverstate = sState;
  rep.infos.serverinfo.players     = nbUsers;
  Net_sendpacket(&rep, slots[which].sock);

  //Send his infos...
  rep.which  = SERVERID;
  rep.type   = USERINFO;
  rep.infos.userinfo.which = which;
  rep.infos.userinfo.ismaster = slots[which].isMaster;
  rep.infos.userinfo.color = 1; //TODO: get right color..

  //TODO : Check for dupplicate login  
  strcpy(slots[which].name, packet.infos.login.nick);
  printf("Connection de %s sur le slot %d\n", slots[which].name, which);

  //TODO: if nick already exists, change and send...
  strcpy(rep.infos.userinfo.nick, slots[which].name); //TODO: get right color..
  Net_sendpacket(&rep, slots[which].sock);
  
  //Send all others login
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if( slots[i].active == 1 )
	{
	  rep.which  = SERVERID;
	  rep.type   = USERINFO;
	  rep.infos.userinfo.which = i;
	  rep.infos.userinfo.ismaster = slots[i].isMaster;
	  rep.infos.userinfo.color = 1; //TODO: get right color..
	  strcpy(rep.infos.userinfo.nick, slots[i].name);
	  Net_sendpacket(&rep, slots[which].sock);
	}
    }
	  
  //Send all users that someone join...
  //To say that, we only need to send a USERINFO
  rep.which = SERVERID;
  rep.type  = USERINFO;
  rep.infos.userinfo.which = which;
  rep.infos.userinfo.ismaster = slots[which].isMaster;
  rep.infos.userinfo.color = slots[which].color;
  strcpy(rep.infos.userinfo.nick, slots[which].name);
  rep2.which = SERVERID;
  rep2.type  = ACTION;
  rep2.infos.action.type = JOIN;
  rep2.infos.action.which= which;

  for(i=0;i<4;++i)
    {
      if( slots[i].active == 1 )
	{
	  Net_sendpacket(&rep2, slots[i].sock);
	  Net_sendpacket(&rep, slots[i].sock);
	}
    }
  slots[which].active = 1;

  //Send NetRules
  rep.which = SERVERID;
  rep.type  = NETRULES;
  rep.infos.netrules.nbWins = netrulenbwins;
  rep.infos.netrules.time   = netruletime;
  printf("Net rules : %d %d\n", netrulenbwins,  netruletime);
  Net_sendpacket(&rep, slots[which].sock);

}

void
do_chat( int which, Packet packet )
{
  int    i;
  Packet rep;
 

  rep.which=which;
  rep.type=CHAT;
  rep.infos.chat.which=packet.infos.chat.which;
  strcpy(rep.infos.chat.mesg, packet.infos.chat.mesg);


  if( packet.infos.chat.which == BROADCAST )
    {
      for(i=0; i<4; ++i)
	{
	if( slots[i].active == 1 )
	  {
	    Net_sendpacket(&rep, slots[i].sock);
	  }
	}
    } else {
      Net_sendpacket(&rep, slots[packet.infos.chat.which].sock);
    }
}

void
do_startgame( int which, Packet packet )
{
  Packet   rep;
  int      i;
  Player *p;
  AI     *ai;

  printf("+ do_startgame\n");
  printf("Starting the game\n");

  //TODO : change all things about init data ( andi will do )

  //initData();
  //Send game rules...

  for(i=0; i< MAX_PLAYERS; ++i)
    {
    p = (game->player + i);
    ai = p->ai;
    ai->active = ( slots[i].active == 1 ) ? 0 : 2;
    ai->tdiff = 0;
    ai->moves = 0;
    ai->danger = 0;
    ai->lastx = 0;
    ai->lasty = 0;
    }

  //resetScores();
  game2->players                   = nbUsers;
  game->players=game2->players;
  initData();



  

  rep.which                        = SERVERID;
  rep.type                         = GAMERULES;
  rep.infos.gamerules.players      = game2->players;
  rep.infos.gamerules.speed        = game2->rules.speed;
  rep.infos.gamerules.eraseCrashed = game2->rules.eraseCrashed;
  rep.infos.gamerules.gamespeed    = game->settings->game_speed;
  rep.infos.gamerules.grid_size    = game->settings->grid_size;
  rep.infos.gamerules.arena_size   = game->settings->arena_size;
  rep.infos.gamerules.time         = game2->time;
  //Startpos
  printf("%d players, getting start post\n", game2->players);
  if( game2->startPositions != NULL )
    free(game2->startPositions);
  game2->startPositions = ( int *)malloc(3*game2->players *sizeof(int));
  for(i=0; i<game2->players; ++i)
    {
      game2->startPositions[3*i+0]=game->player[i].data->iposx;
      game2->startPositions[3*i+1]=game->player[i].data->iposy;
      game2->startPositions[3*i+2]=game->player[i].data->dir;
    }
  
  printf("%d players, preparing start post\n", game2->players);
  for(i=0; i<game2->players; ++i)
    {
      rep.infos.gamerules.startPos[3*i+0]=game->player[i].data->iposx;
      rep.infos.gamerules.startPos[3*i+1]=game->player[i].data->iposy;
      rep.infos.gamerules.startPos[3*i+2]=game->player[i].data->dir;
    }
  for(i=0; i<4; ++i)
    {
      if( slots[i].active == 1 )
	{
	  Net_sendpacket(&rep, slots[i].sock);
	}
    }

  //Send a change state	  
  sState = gameState;
  rep.which=SERVERID;
  rep.type =SERVERINFO;
  rep.infos.serverinfo.serverstate = sState;
  rep.infos.serverinfo.players     = nbUsers; // may be game2->players?
 
  for(i=0; i<4; ++i)
    {
      if( slots[i].active )
	{
	Net_sendpacket(&rep, slots[i].sock);
	}
    }
  game2->events.next = NULL;
  game2->mode = GAME_SINGLE;
  game2->players = nbUsers;
  game->players=game2->players;
  printf("starting game with %d players\n", game->players); 
  printf("- do_startgame\n");
}

void
do_action( int which, Packet packet )
{
  printf("+ do_action\n");
  switch( packet.infos.action.type )
    {
    case TURNRIGHT:
      printf("get turn left from %d\n", which);
      createTurnEvent(which, TURN_RIGHT);
      break;
    case TURNLEFT:
      printf("get turn left from %d\n", which);
      createTurnEvent(which, TURN_LEFT);
      break;
    case STARTGAME:
      do_startgame(which, packet);
      break;
    default:
      fprintf(stderr, "Received an action packet with a type %d that not be allowed or unknown\n", packet.infos.action.type);
      break;
    }
  printf("- do_gameState\n");
}

void
do_waitState( int which, Packet packet )
{
  printf("+ do_waitState\n");
  switch( packet.type )
    {
    case LOGIN:
      do_login(which, packet);
      break;
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the waitState\n", packet.type);
      break;
    }
  printf("- do_waitState\n");
}

void
do_preGameState( int which, Packet packet )
{
  printf("+ do_preGameState\n");
  switch( packet.type )
    {
    case LOGIN:
      do_login(which, packet);
      break;
    case CHAT:
      do_chat(which, packet);
      break;
    case ACTION:
      do_action(which, packet);
      break;
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the preGameState\n", packet.type);
      break;
    }
  printf("- do_preGameState\n");
}

void
do_gameState( int which, Packet packet )
{
  printf("+ do_gameState\n");
  switch( packet.type )
    {
    case LOGIN:
      do_login(which, packet);
      break;
    case ACTION:
      do_action(which, packet);
      break;
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the preGameState\n", packet.type);
      break;
    }
  printf("- do_gameState\n");
}

void
handle_client(int which)
{
  Packet         packet;

  //Get the packet...
  if( Net_receivepacket(&packet, slots[which].sock) != 0 )
    {
      //Connection perdu
      do_lostplayer(which);
      return;
    }
  
  printf("+ handle_client. Type %d, Which %d\n", packet.type, packet.which);
  switch( sState )
    {
    case waitState:
      do_waitState(which, packet);
      break;
    case preGameState:
      do_preGameState(which, packet);
      break;
    case gameState:
      do_gameState(which, packet);
      break;
    default:
      fprintf(stderr, "Unknown server state %d\n", sState);
    }
  printf("- handle_client\n");
}



void
handle_connection()
{
  TCPsocket   newsock = NULL;
  int         which;
  Packet      packet;
  
  //Accept the new connection
  newsock = SDLNet_TCP_Accept( Net_getmainsock() );
  if( newsock == NULL )
    {
      fprintf(stderr, "Can't Accept connection: %s\n", SDLNet_GetError());
      return;
    }

  //Looking for a free slot
  for(which=0; which < MAX_PLAYERS; ++which)
    {
      if( ! slots[which].sock )
	break;
    }
  
  //If which == MAX_PLAYERS, there is no free slots...
  if( which >= MAX_PLAYERS )
    {
      //We must refuse the connection
      fprintf(stderr, "server is full, refuse login..\n");

      //Advice the client that the server is full...
      packet.type     = LOGINREP; //send before getting a LOGIN! 
      packet.which    = SERVERID;
      packet.infos.loginrep.accept = 0;
      strcpy(packet.infos.loginrep.message, "server is full, try again later...");

      //We close the connection
      Net_closesock(newsock);
    } else {
      //Server is not full...
      //Savings slots
      slots[which].sock = newsock;
      slots[which].peer = *SDLNet_TCP_GetPeerAddress(newsock); //Get remote address
      slots[which].active = -1;

      Net_addsocket( slots[which].sock );
      printf("New Connection on slot %d\n", which);
      switch( which )
	{
	case 0:
	  game->settings->ai_player1 = 0;
	  break;
	case 1:
	  game->settings->ai_player2 = 0;
	  break;
	case 2:
	  game->settings->ai_player3 = 0;
	  break;
	case 3:
	  game->settings->ai_player4 = 0;
	  break;
	}
    }
}



void
handle_server()
{
  int i;

  if( Net_checksocks() )
  {
    printf("has a connection...\n");
    handle_connection();
  }
 
  //look what is said!
  for(i=0;i<MAX_PLAYERS;++i)
    {
      if( Net_readysock(slots[i].sock) )
	{
	  //i said something, handle...
	  handle_client(i);
	}
    }
}

void
SendEvents(GameEvent *e)
{
  int        i;
  Packet     rep;

  rep.which = SERVERID;
  rep.type  = EVENT;
  rep.infos.event.event = *e;

  for( i=0; i <4; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
}
