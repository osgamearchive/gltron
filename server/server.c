#include "server_gltron.h"
#include <limits.h>

static Uint32 ping       = 0;
static Uint32 savedtime  = 0; 
static Uint32 timeout       = 0;
static Uint32 lastping       = 0;
/* static Uint32 slowest    = RAND_MAX; */
static int    slowest       = 0;
static int    getpingrep    = 0;
static int    starting      = 0;
static Uint32 starttime     = 0;
static int    timetostart   = 0;


static int  getping();
static void makeping();

void
start_server()
{
  int        i;


  //Init the slots
  for(i=0;i<MAX_PLAYERS;++i)
    {
      slots[i].active = 0;   //slots is inactive
      slots[i].sock   = 0;   //So no sock..
      slots[i].packet = HEADER;
      slots[i].player = -1;  //means no player found
      slots[i].ping   = 0;
      slots[i].hasstarted = 0;
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
  nbUsers  = 0;

  eventList = createEventlist();
}

void
stop_server()
{
  Net_cleanup();
  //TODO: See how things work when brutal disconnect? why not say to player that server will shutdown
  //May be with a message ( eg: server maintenance, etc... )
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

      printf("%s part.\n", slots[which].name);
      //TODO: it still a bug here,
      //What to do if player 1 for example leave


      //TODO: if there is no1 left, init game to default #done
      //and go to waitState.
      if( nbUsers == 0 )
	{
	  printf("Nobody left: swith to default setting and state is waitState...\n");
	  //change to waitState
	  sState=waitState;	  

	  //init NetRules
	  netrulenbwins = 5;
	  netruletime   = 0;
	  initDefaultSettings();


	  //Change game mode... nothing to do with game things...
	  game2->mode = GAME_NETWORK_RECORD;
	  hasstarted=0;
	  lastping = 0;
	  slowest    = INT_MAX;
	  starting = 0;
	} else {
	  
	  //TODO: If it was the game master, change that #done
	  if( slots[which].isMaster )
	    {
	      //In this case, we need a new master, otherwise game is blocked
	      //And no1 can start game.
	      
	      //search for an active slot
	      i=-1;
	      while( slots[++i].active != 1 && i <= MAX_PLAYERS ) { }
	      //i is our new master
	      slots[i].isMaster=1;
	      
	      //Say to other...
	      rep.which=SERVERID;
	      rep.type=ACTION;
	      rep.infos.action.type=CHGEGAMEMASTER;
	      rep.infos.action.which=i;	 //Means i is the new Game Master
	        
	      printf("Changing Game Master, new one is %d", rep.infos.action.which);

	      for(i=0; i< 4; ++i)
		{
		  if( slots[i].active == 1 )
		    {	 
		      Net_sendpacket(&rep, slots[i].sock);
		    }
		}	      
	    }
	  
	  //Say to other...
	  rep.which=SERVERID;
	  rep.type=ACTION;
	  rep.infos.action.type=PART;
	  rep.infos.action.which=which;	  
	  for(i=0; i< 4; ++i)
	    {
	      if( slots[i].active == 1 )
		{	 
		  Net_sendpacket(&rep, slots[i].sock);
		}
	    }
	}

    }
  Net_delsocket(slots[which].sock);
  Net_closesock(slots[which].sock);
  slots[which].sock=NULL;

  //Reset score
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      netscores.points[i]=0;
    }
}

int
check_version(char *vers )
{
  //TODO: check version. #done

#define VERSION "0.61am" 
  //Check for curent version 0.61am
  printf("checking version %s <> %s\n", vers, VERSION);
  return ! strcmp(vers, VERSION);
}

void
do_login( int which, Packet packet )
{
  Packet rep, rep2;
  int    i, j;

  if( ! check_version(packet.infos.login.version) )
    {
      printf("wrong version: %s\n", packet.infos.login.version);
      //Send the reponse deny...
      rep.which  = SERVERID;
      rep.type   = LOGINREP;
      rep.infos.loginrep.accept = 0;
      strcpy(rep.infos.loginrep.message, "wrong gltron version.\n");
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

  //TODO Search for the game master... : is it necessary?

  //Accept the connection
  rep.which  = which;
  rep.type   = LOGINREP;
  rep.infos.loginrep.accept = 1;
  strcpy(rep.infos.loginrep.message, "Welcome to gltron network.\n");
  printf("+ sending loginrep\n");
  Net_sendpacket(&rep, slots[which].sock);
  printf("- sending loginrep\n");

  //Changing server state...
  sState = preGameState;

  if( hasstarted )
    {
      //game has already started, this player must be inform that it already has started!      
      rep.which = SERVERID;
      rep.type  = ACTION;
      rep.infos.action.type=HASSTARTED;
      Net_sendpacket(&rep, slots[which].sock);      
      sState = gameState;
    }


  nbUsers++;

  //Send server infos...
  rep.which   = SERVERID;
  rep.type    = SERVERINFO;
  if( ! hasstarted )
    rep.infos.serverinfo.serverstate = sState;
  else
    rep.infos.serverinfo.serverstate = preGameState;
  rep.infos.serverinfo.players       = nbUsers;
  Net_sendpacket(&rep, slots[which].sock);

  //Send his infos...
  rep.which  = SERVERID;
  rep.type   = USERINFO;
  rep.infos.userinfo.which = which;
  rep.infos.userinfo.ismaster = slots[which].isMaster;
  rep.infos.userinfo.color = 1; //TODO: get right color...

  //TODO : Check for duplicate login #done
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if( ( slots[i].active == 1 ) && (! strcmp(packet.infos.login.nick, slots[i].name)) )
	break;
    }
  if( i < MAX_PLAYERS )
    {
      //Someone already have this nickname 
      i=1+(int) (100.0*rand()/(RAND_MAX+1.0));   
      sprintf(slots[which].name, "%s%d", packet.infos.login.nick, i);
    } else {      
      strcpy(slots[which].name, packet.infos.login.nick);
    }

  printf("Connection from %s one slot %d\n", slots[which].name, which);

  //TODO: if nick already exists, change and send... #done
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

  //Send init GameSet
  rep.which = SERVERID;
  rep.type  = GAMESET;
  rep.infos.gameset.eraseCrashed =   game->settings->erase_crashed;
  rep.infos.gameset.gamespeed    = game->settings->game_speed;
  rep.infos.gameset.arena_size   = game->settings->arena_size;
  printf("GameSet : %d %d %d\n", packet.infos.gameset.gamespeed,
	 packet.infos.gameset.eraseCrashed,
	 packet.infos.gameset.arena_size);
  Net_sendpacket(&rep, slots[which].sock);
  lastping=SystemGetElapsedTime();

  if( hasstarted == 1 )
    {

   /*    for(i=0; i<MAX_PLAYERS; ++i) */
/* 	{ */
/* 	  slots[i].player     = -1; */
/* 	  slots[i].hasstarted = 0; */
/* 	} */
      
/*       for(i=0; i<MAX_PLAYERS; ++i) */
/* 	{ */
/* 	  if( slots[i].active == 1 ) */
/* 	    { */
/* 	      //find the new player ident */
/* 	      slots[i].player = find_freeplayer(); */
/* 	      printf("new player %d -> %d\n", i, slots[i].player); */
/* 	    } else { */
/* 	      slots[i].player = -1; */
/* 	    } */
/* 	} */

      //Send rules as we are starting the game...
      rep.which                        = SERVERID;
      rep.type                         = GAMERULES;
      rep.infos.gamerules.players      = game2->players+1;
      rep.infos.gamerules.speed        = game2->rules.speed;
      rep.infos.gamerules.eraseCrashed = game->settings->erase_crashed;
      rep.infos.gamerules.gamespeed    = game->settings->game_speed;
      rep.infos.gamerules.grid_size    = game->settings->grid_size;
      rep.infos.gamerules.arena_size   = game->settings->arena_size;
      rep.infos.gamerules.time         = game2->time;
      //Startpos
      printf("%d players, preparing start post\n", game2->players+1);
      rep2.which                        = SERVERID;
      rep2.type                         = STARTPOS;
      for(i=0; i<game2->players; ++i)
	{
	  
	  j=getWhich(i);
	  printf("\nget startpos client ( sent ) %d <-> server %d\n", j, i);
/* 	  rep2.infos.startpos.startPos[3*j+0]=game->player[i].data->iposx; */
/* 	  rep2.infos.startpos.startPos[3*j+1]=game->player[i].data->iposy; */
/* 	  rep2.infos.startpos.startPos[3*j+2]=game->player[i].data->dir; */
	  rep2.infos.startpos.startPos[3*j+0]=game2->startPositions[3*i];
	  rep2.infos.startpos.startPos[3*j+1]=game2->startPositions[3*i+1];
	  rep2.infos.startpos.startPos[3*j+2]=game2->startPositions[3*i+2];
	  
	  printf("\n\npos %d %d %d\n\n\n", rep2.infos.startpos.startPos[3*j+0],
		 rep2.infos.startpos.startPos[3*j+1],
		 rep2.infos.startpos.startPos[3*j+2]);
	}
      printf("sending game rules and startpos to %d\n", i);
      Net_sendpacket(&rep, slots[which].sock);
      Net_sendpacket(&rep2, slots[which].sock);
      
      //Send server infos...
      rep.which   = SERVERID;
      rep.type    = SERVERINFO;
      rep.infos.serverinfo.serverstate = sState;
      rep.infos.serverinfo.players     = nbUsers;
      Net_sendpacket(&rep, slots[which].sock);
      sendEventlist(eventList, which);
    }
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
  Packet   rep, rep2;
  int      i, j;

  if( ! slots[which].isMaster )
    return;

  netscores.winner=-1;
  initEventlist(eventList);

  //TODO: clean this part of code. Really UGLY...


  printf("+ do_startgame\n");
  printf("Starting the game\n");

  //TODO: change player so the game can continue if players has left...
  //for each player, rearange player ident
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      slots[i].player     = -1;
      slots[i].hasstarted = 0;
    }
  
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if( slots[i].active == 1 )
	{
	  //find the new player ident
	  slots[i].player = find_freeplayer();
	  printf("new player %d -> %d\n", i, slots[i].player);
	} else {
	  slots[i].player = -1;
	}
    }

  //TODO : change all things about init data ( andi will do )

 
  /** before doing initData check for active players */
  for(i=0; i<MAX_PLAYERS;  ++i)
    {
      j = getWhich(i);
      
      game->player[i].ai->active = ( (j != -1) && (slots[j].active==1) ) ? 0 : 2;
      printf("activating player %d on slot %d : %d\n", i, getWhich(i), game->player[i].ai->active);
    }

  game->players = nbUsers;
  //
  applyGameInfo();
  initData();

  //resetScores();


  rep.which                        = SERVERID;
  rep.type                         = GAMERULES;
  rep.infos.gamerules.players      = game2->players;
  rep.infos.gamerules.speed        = game2->rules.speed;
  rep.infos.gamerules.eraseCrashed = game->settings->erase_crashed;
  rep.infos.gamerules.gamespeed    = game->settings->game_speed;
  rep.infos.gamerules.grid_size    = game->settings->grid_size;
  rep.infos.gamerules.arena_size   = game->settings->arena_size;
  rep.infos.gamerules.time         = game2->time;
  //Startpos
  printf("%d players, preparing start post\n", game2->players);
  rep2.which                        = SERVERID;
  rep2.type                         = STARTPOS;

  if( game2->startPositions == NULL )
    {
      game2->startPositions = (int*) malloc( 3*MAX_PLAYERS*sizeof(int));
    }

  for(i=0; i<game2->players; ++i)
    {
	 
	  j=getWhich(i);
	  printf("\nget startpos client ( sent ) %d <-> server %d\n", j, i);
	  rep2.infos.startpos.startPos[3*j+0]=game->player[i].data->iposx;
	  rep2.infos.startpos.startPos[3*j+1]=game->player[i].data->iposy;
	  rep2.infos.startpos.startPos[3*j+2]=game->player[i].data->dir;

	  game2->startPositions[3*i]  = game->player[i].data->iposx;
	  game2->startPositions[3*i+1]= game->player[i].data->iposy;
	  game2->startPositions[3*i+2]= game->player[i].data->dir;
	  printf("\n\npos %d %d %d\n\n\n", rep2.infos.startpos.startPos[3*j+0],
		 rep2.infos.startpos.startPos[3*j+1],
		 rep2.infos.startpos.startPos[3*j+2]);
    }

  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if( slots[i].active == 1 )
	{
	  printf("sending game rules and startpos to %d\n", i);
	  Net_sendpacket(&rep, slots[i].sock);
	  Net_sendpacket(&rep2, slots[i].sock);
	}
    }  
  slowest=0;
  //find slowest ping
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      if (slots[i].active == 1)
	{
	  //printf("get i=%d %d < %d\n", i, SDLNet_Read16(&slots[i].ping), slowest );
	  if( slots[i].ping > slowest ){
	    slowest = slots[i].ping;
	  }
	}
    }
  
  
  printf("slowest ping : %d\n", slowest);
  starting=1;
  timetostart=-slowest;
  starttime = SystemGetElapsedTime();

  //Send a change state	  
/*   sState = gameState; */
/*   rep.which=SERVERID; */
/*   rep.type =SERVERINFO; */
/*   rep.infos.serverinfo.serverstate = sState; */
/*   rep.infos.serverinfo.players     = nbUsers; // may be game2->players? */
 
/*   for(i=0; i<MAX_PLAYERS; ++i) */
/*     { */
/*       if( slots[i].active == 1 ) */
/* 	{ */
/* 	  if( slots[i].isMaster == 1 ) */
/* 	    makeping(); */
/* 	  Net_sendpacket(&rep, slots[i].sock); */
/* 	} */
/*     } */
  /**
  if( ! hasstarted )
    hasstarted = 1;
  game2->events.next = NULL;
  game2->mode = GAME_SINGLE;
  printf("starting game with %d players\n", game->players); 
  printf("- do_startgame\n");
  printf("\n\npos Player 1 is %d %d %d\n\n\n", game->player[0].data->iposx,
	 game->player[0].data->iposy, game->player[0].data->dir);
  **/
}

void
do_startconfirm(int which, Packet packet)
{
  int time=packet.infos.action.which/2;

  makeping();
  printf("+ ping start -> confirm = %d at %d\n", getping(), game2->time.current);

  if( slots[which ].isMaster != 1 )
    return;



  game2->time.lastFrame = time;
  game2->time.current   = time;
  game2->time.offset    = SystemGetElapsedTime();

  if( ! hasstarted )
    {
    timeout             = SystemGetElapsedTime();
    hasstarted = 1;
    }
  game2->events.next = NULL;
  game2->mode = GAME_SINGLE;
  printf("starting game with %d players at %d\n", game->players, game2->time.current); 
  printf("- do_startgame\n");
  printf("\n\npos Player 1 is %d %d %d\n\n\n", game->player[0].data->iposx,
	 game->player[0].data->iposy, game->player[0].data->dir);
}

void
do_chgenbwins(int which, Packet packet)
{
  Packet rep;
  int    i;

  if( ! slots[which].isMaster )
    return;

  if( sState != preGameState )
    return;

  netrulenbwins = packet.infos.action.which;

  //Send new NetRules to users...
  rep.which = SERVERID;
  rep.type  = NETRULES;
  rep.infos.netrules.nbWins = netrulenbwins;
  rep.infos.netrules.time   = netruletime;
  printf("Net rules : %d %d\n", netrulenbwins,  netruletime);
  for(i=0; i<MAX_PLAYERS; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
}


void
do_chgetimeout(int which, Packet packet)
{
  Packet rep;
  int    i;

  if( ! slots[which].isMaster )
    return;

  if( sState != preGameState )
    return;

  netruletime = packet.infos.action.which;

  //Send new NetRules to users...
  rep.which = SERVERID;
  rep.type  = NETRULES;
  rep.infos.netrules.nbWins = netrulenbwins;
  rep.infos.netrules.time   = netruletime;
  printf("Net rules : %d %d\n", netrulenbwins,  netruletime);
  for(i=0; i<MAX_PLAYERS; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
}

void
do_chgespeed(int which, Packet packet)
{
  Packet rep;
  int    i;

  if( ! slots[which].isMaster )
    return;

  if( sState != preGameState )
    return;

  if( packet.infos.action.which < 0 ||
      packet.infos.action.which > 3 )
    return;

  game->settings->game_speed  = packet.infos.action.which;

  //Send new NetRules to users...
  rep.which = SERVERID;
  rep.type  = GAMESET;
  rep.infos.gameset.eraseCrashed = game->settings->erase_crashed;
  rep.infos.gameset.gamespeed    = game->settings->game_speed;
  rep.infos.gameset.arena_size   = game->settings->arena_size;
  printf("GameSet : %d %d %d\n", packet.infos.gameset.gamespeed,
	 packet.infos.gameset.eraseCrashed,
	 packet.infos.gameset.arena_size);
  for(i=0; i<MAX_PLAYERS; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
}

void
do_chgesize(int which, Packet packet)
{
  Packet rep;
  int    i;

  if( ! slots[which].isMaster )
    return;

  if( sState != preGameState )
    return;

  if( packet.infos.action.which < 0 ||
      packet.infos.action.which > 4 )
    return;

  game->settings->arena_size  = packet.infos.action.which;

  //Send new NetRules to users...
  rep.which = SERVERID;
  rep.type  = GAMESET;
  rep.infos.gameset.eraseCrashed = game->settings->erase_crashed;
  rep.infos.gameset.gamespeed    = game->settings->game_speed;
  rep.infos.gameset.arena_size   = game->settings->arena_size;
  printf("GameSet : %d %d %d\n", packet.infos.gameset.gamespeed,
	 packet.infos.gameset.eraseCrashed,
	 packet.infos.gameset.arena_size);
  for(i=0; i<MAX_PLAYERS; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
}

void
do_chgeerase(int which, Packet packet)
{
  Packet rep;
  int    i;

  if( ! slots[which].isMaster )
    return;

  if( sState != preGameState )
    return;

  if( packet.infos.action.which != 0 &&
      packet.infos.action.which != 1 )
    return;
  game->settings->erase_crashed = packet.infos.action.which;
  game->settings->erase_crashed     = packet.infos.action.which;

  //Send new NetRules to users...
  rep.which = SERVERID;
  rep.type  = GAMESET;
  rep.infos.gameset.eraseCrashed = game->settings->erase_crashed;
  rep.infos.gameset.gamespeed    = game->settings->game_speed;
  rep.infos.gameset.arena_size   = game->settings->arena_size;
  printf("GameSet : %d %d %d\n", packet.infos.gameset.gamespeed,
	 packet.infos.gameset.eraseCrashed,
	 packet.infos.gameset.arena_size);
  for(i=0; i<MAX_PLAYERS; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
}

void
do_pingrep( int which, Packet packet )
{
  short ping;
  //doing average: is this good?
/*   slots[which].ping+=(SystemGetElapsedTime() - packet.infos.action.which); */
/*   slots[which].ping/=2; */
  //slots[which].ping = SystemGetElapsedTime() - packet.infos.action.which;
  ping = SystemGetElapsedTime() - packet.infos.action.which;
  slots[which].ping+=ping;
  slots[which].ping/=2;
  printf("ping is %d\n", slots[which].ping);  
  getpingrep++;
}


void
do_action( int which, Packet packet )
{
  printf("+ do_action\n");
  switch( packet.infos.action.type )
    {
    case TURNRIGHT:
      printf("get turn right from %d -> %d (time %d)\n", which, getPlayer(which), game2->time.current);
      createTurnEvent(getPlayer(which), TURN_RIGHT);
      break;
    case TURNLEFT:
      printf("get turn left from %d -> %d (time %d)\n", which, getPlayer(which), game2->time.current);
      createTurnEvent(getPlayer(which), TURN_LEFT);
      break;
    case STARTGAME:
      do_startgame(which, packet);
      break;
/*     case CONFSTART: */
/*       do_startconfirm(which, packet); */
      break;
    case CHGENBWINS:
      do_chgenbwins(which, packet);
      break;
    case CHGETIMEOUT:
      do_chgetimeout(which, packet);
      break;
    case CHGESPEED:
      do_chgespeed(which, packet);
      break;
    case CHGESIZE:
      do_chgesize(which, packet);
      break;
    case CHGEERASE:
      do_chgeerase(which, packet);
      break;
    case PING:
      //it's a ping reply
      do_pingrep(which, packet);
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
  //makeping(0);
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
  //makeping(1);
  //printf("## do_preGameState took %d ms\n", getping()); 
  printf("- do_preGameState\n");
}

void
do_gameState( int which, Packet packet )
{
  printf("+ do_gameState\n");
  //makeping(0);
  switch( packet.type )
    {
    case LOGIN:
      do_login(which, packet);
      break;
    case ACTION:
      do_action(which, packet);
      break;
    case CHAT:
      do_chat(which, packet);
      break;
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the preGameState\n", packet.type);
      break;
    }
  //makeping(1);
  //printf("## do_gameState took %d ms\n", getping()); 
  printf("- do_gameState\n");
}

void
handle_client(int which)
{
  Packet         packet;
  int            type = slots[which].packet;

  //Get the packet...
  if( Net_receivepacket(&packet, slots[which].sock, which, type) != 0 )
    {
      //Connection perdu
      do_lostplayer(which);
      return;
    }

  if( type == HEADER ) //It's an header, do not need to handle things;..
    return;
  
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
      slots[which].packet = HEADER;

      //Find a free player
      slots[which].player = find_freeplayer();
      printf("player %d is know by server has player %d\n", which, slots[which].player);

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
  int   i;

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
	  //makeping();
	  handle_client(i);
	  //makeping();
	  //printf("## handle_client took %d ms\n", getping());
	}
    }
}

void
do_starting()
{
  Packet rep;
  int    i;
  
  
  if( starting == 1 )
    {

      sState = gameState;
      rep.which=SERVERID;
      rep.type =SERVERINFO;
      rep.infos.serverinfo.serverstate = sState;
      rep.infos.serverinfo.players     = nbUsers;

      printf("timetostart %d\n", timetostart);
      if( timetostart > 0 )
	{
	  //start the server
	  starting=0;
	  game2->time.lastFrame = 0;
	  game2->time.current   = 0;
	  game2->time.offset    = SystemGetElapsedTime();
	  
	  if( ! hasstarted )
	    {
	      timeout             = SystemGetElapsedTime();
	      hasstarted = 1;
	    }
	  game2->events.next = NULL;
	  game2->mode = GAME_SINGLE;
	  printf("starting game with %d players at %d\n", game->players, game2->time.current); 
	  printf("- do_startgame\n");
	  printf("\n\npos Player 1 is %d %d %d\n\n\n", game->player[0].data->iposx,
		 game->player[0].data->iposy, game->player[0].data->dir);

	} else {
	  for(i=0; i<MAX_PLAYERS; ++i)
	    {
	      if( slots[i].active == 1 && timetostart >= (slots[i].ping*-1) && slots[i].hasstarted == 0 )
		{
	      printf("%d >= %d\n", timetostart, slots[i].ping );
		  slots[i].hasstarted=1;
		  printf("sending signal to %s\n", slots[i].name);
		  //send him signal to start
		   Net_sendpacket(&rep, slots[i].sock);
		} 
	    }
	}
      timetostart+=SystemGetElapsedTime()-starttime;
  /**
  if( ! hasstarted )
    hasstarted = 1;
  game2->events.next = NULL;
  game2->mode = GAME_SINGLE;
  printf("starting game with %d players\n", game->players); 
  printf("- do_startgame\n");
  printf("\n\npos Player 1 is %d %d %d\n\n\n", game->player[0].data->iposx,
	 game->player[0].data->iposy, game->player[0].data->dir);
  **/
    }
}

void
do_ping_users()
{
  Packet packet;
  int    i;

  if( lastping == 0 )
    lastping=SystemGetElapsedTime();

  if( (SystemGetElapsedTime() - lastping)/1000 > 5 )
    {
      packet.which               = SERVERID;
      packet.type                = ACTION;
      packet.infos.action.type   = PING;
      packet.infos.action.which  = SystemGetElapsedTime();
      for(i=0; i< MAX_PLAYERS; ++i )
	{
	  if( slots[i].active == 1 )
	    {
	      Net_sendpacket(&packet, slots[i].sock);
	    }
	}
      lastping=SystemGetElapsedTime();
    }
  if( getpingrep >= nbUsers )
    {
      packet.type                = PLAYERSPING;
      for(i=0; i< MAX_PLAYERS; ++i )
	packet.infos.playersping.ping[i] = slots[i].ping;
      for(i=0; i< MAX_PLAYERS; ++i )
	{
	  if( slots[i].active == 1 )
	    {
	      Net_sendpacket(&packet, slots[i].sock);
	    }
	}
      getpingrep=0;
    }
}

void
SendEvents(GameEvent *e)
{
  int        i, real;
  Packet     rep;


  rep.which = SERVERID;
  rep.type  = EVENT;
  real      = e->player; 

  //Real bad coding, hum nico, u should do better 'cause dangerous!!!
  e->player = getWhich(e->player); //doing the equivalence
  rep.infos.event.event = *e;

  for( i=0; i <MAX_PLAYERS; ++i)
      if( slots[i].active == 1 )
	Net_sendpacket(&rep, slots[i].sock);
  e->player = real;
}


int
getPlayer(int which)
{
  return slots[which].player;
}

int
find_freeplayer()
{
  int i,j;
  
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      //look if player i is free
      for(j=0; j<MAX_PLAYERS; ++j)
	{
	  if( slots[j].player == i )
	    break;
	}
      if( j == MAX_PLAYERS )
	{
	  //player i is free we use it!
	  break;
	}
    }
  return i;
}


int
getWhich(int player)
{
  int i;
  for(i=0; i<MAX_PLAYERS;++i)
    {
      if( slots[i].player == player )
	{
	  fprintf(stderr, "getWich %d -> %d\n", player, i);
	  return i;
	}
    }
  return -1;
}

void
do_timeout( )
{
  Packet rep;
  int    winner=0, max, i;

  if( SystemGetElapsedTime() - timeout > 1000 * 60 * netruletime )
    {
      //Time is Up! means end game 

      //Search for winner
      max=0;
      for(i=0;i<MAX_PLAYERS;++i)
	{
	  if( netscores.points[i] > max )
	    {
	      max = netscores.points[i];
	      winner = i;
	    }
	}

     
      netscores.winner=winner;
      printf("winner is %d\n", winner);

      //Sending scores to every one.
      rep.which = SERVERID;
      rep.type  = SCORE;
      rep.infos.score.winner=netscores.winner;
      //memcpy(rep.infos.score.points, netscores.points, 4*MAX_PLAYERS);
      for(i=0;i<MAX_PLAYERS;++i)
	{
	  rep.infos.score.points[i] = ( slots[i].active==1 ) ? netscores.points[i]:0;
	  if( slots[i].active==1 )
	    printf("%s (slot %d) has %hd points\n", slots[i].name, i, rep.infos.score.points[i]);
	}

      for(i=0; i<MAX_PLAYERS; ++i)
	{
	  if( slots[i].active )
	    {
	      Net_sendpacket(&rep, slots[i].sock);
	    }
	}
      
      //Init netscores for next games
      netscores.winner=-1;
      for(i=0; i<MAX_PLAYERS; ++i)
	{
	  netscores.points[i]=0;
	}
      hasstarted=0;
      

      game->pauseflag = PAUSE_GAME_FINISHED;
      //start a new game...
      
      game2->mode = GAME_NETWORK_RECORD;
      //go to pregame state...
      sState = preGameState;
      
      rep.which=SERVERID;
      rep.type=SERVERINFO;
      rep.infos.serverinfo.serverstate=preGameState;
      for(i=0; i<4; ++i)
	{
	  if( slots[i].active )
	    {
	      Net_sendpacket(&rep, slots[i].sock);
	    }
	}
    }
}

void
do_wingame( int winner)
{
  Packet rep;
  int    i;
  int    max;

  //TODO: time rules..


  //Change Points
  netscores.points[getWhich(winner)]++;

  //See if it's finished?

  //Search for each player max points
  max=0;
  for(i=0;i<MAX_PLAYERS;++i)
    {
      if( netscores.points[i] > max )
	max = netscores.points[i];
    }

  if( netruletime == 0 &&  max >= netrulenbwins )
    {
      //The game is finished
      netscores.winner=getWhich(winner);
      printf("winner is %d\n", winner);

      //Sending scores to every one.
      rep.which = SERVERID;
      rep.type  = SCORE;
      rep.infos.score.winner=netscores.winner;
      //memcpy(rep.infos.score.points, netscores.points, 4*MAX_PLAYERS);
      for(i=0;i<MAX_PLAYERS;++i)
	{
	  rep.infos.score.points[i] = ( slots[i].active==1 ) ? netscores.points[i]:0;
	  if( slots[i].active==1 )
	    printf("%s (slot %d) has %hd points\n", slots[i].name, i, rep.infos.score.points[i]);
	}


      for(i=0; i<MAX_PLAYERS; ++i)
	{
	  if( slots[i].active )
	    {
	      Net_sendpacket(&rep, slots[i].sock);
	    }
	}
      
      //Init netscores for next games
      netscores.winner=-1;
      for(i=0; i<MAX_PLAYERS; ++i)
	{
	  netscores.points[i]=0;
	}
      hasstarted=0;

    }

  game->pauseflag = PAUSE_GAME_FINISHED;
  //start a new game...
  
  game2->mode = GAME_NETWORK_RECORD;
  //go to pregame state...
  sState = preGameState;
  
  rep.which=SERVERID;
  rep.type=SERVERINFO;
  rep.infos.serverinfo.serverstate=preGameState;
  for(i=0; i<4; ++i)
    {
      if( slots[i].active )
	{
	  Net_sendpacket(&rep, slots[i].sock);
	}
    }
}


static int
getping()
{
  return ping;
}

static void
makeping()
{
  if( savedtime == 0 )
    {
      savedtime = SystemGetElapsedTime();
      ping=0;
    } else {
      ping = SystemGetElapsedTime() - savedtime;
      savedtime=0;
    }
}
