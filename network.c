#include "gltron.h"

/**********************************************************************************************
 *                                                                                            *
 *  This file is part of gltron project.                                                      *
 *                                                                                            *
 *  Main network layer.                                                                       *
 *                                                                                            *
 *  Author : Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )                                   *
 *                                                                                            *
 *********************************************************************************************/

//Consts---------------------------------------------------------------------------------------
static const int chatcolor[4] = {4, 2, 5, 0};

//Globals--------------------------------------------------------------------------------------
static Uint32 ping[5]       = {0, 0, 0, 0, 0};
static Uint32 savedtime     = 0;
static int    observerstate = 0;
static Sint32 now;
static Sint32 base;

//Enums----------------------------------------------------------------------------------------
enum
  {
    ASKED,         //the turn has been asked to the server
    PREDICTED      //the client has made the turn ( perdiction )    
  };

//Typedefs-------------------------------------------------------------------------------------
typedef struct Predictedturn *Predictedturn;
typedef struct Predictedturn {
  int            dir;     //dir turn asked
  int            rdir;    //real direction after the turn
  int            time;
  int            statut;
  Predictedturn  next;
} predictedturn;

typedef struct TurnList {
  Predictedturn  head;
} TurnList;

TurnList *turnlist;



//Prototypes------------------------------------------------------------------------------------
void free_turn(Predictedturn turn);


static void printSynchPacket( Packet p ) {
  if( p.type != SYNCH )
    return;
  
  printf("\nSynchpacket %8p:\n", &p );
  printf("\ttype=%d\n",p.infos.synch.type);
  printf("\tdata.u.s=%u\n",p.infos.synch.data.u.s);
  printf("\tdata.u.c=%u\n",p.infos.synch.data.u.c);
  printf("\tdata.s.s=%d\n",p.infos.synch.data.s.s);
  printf("\tdata.s.c=%d\n",p.infos.synch.data.s.c);
}


//----------------------------------------------------------------------------------------------

/**
 *  Make and send the login packet.
 *  name is the nickname that want to be used in-game.
 */
void
login(char *name)
{
  int           i=0;
  Packet        packet;
  
  makeping(i);
 
  //Prepare slots for all players 'cause if login is successfull, we'll get
  //all informations about connected users from the server
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      slots[i].active  = 0;
      slots[i].packet  = HEADER;    
      slots[i].player  = -1;   
      slots[i].ping    = 0;
    }

  packet_type = HEADER;

  if( Net_getmainsock()!= NULL  )
    {
      //Sending welcome to the server.
      //TODO: put gltron version here...
      packet.which=me;
      packet.type=LOGIN;
      strcpy(packet.infos.login.version, VERSION);
      strcpy(packet.infos.login.nick, name);
#ifdef DEBUG
      printf("+ send login\n");
#endif
      Net_sendpacket(&packet, Net_getmainsock());
#ifdef DEBUG
      printf("- send login\n");
#endif
    }

  //Create the net event list
  neteventlist = createNetEventList();
  hasstarted = 0;
}

/**
 *  if the connection is lost, we clean the connection and go back to the main menu.
 */
void
connectionLost()
{
  fprintf(stderr, "connection lost\n");
  isConnected=0;
  isLogged=0;
  Net_disconnect();
  serverstate=preGameState; //We hope that next time server will be to preGameState
  
  changeCallback(&trackerscreenCallbacks, &guiCallbacks);
  //switchCallbacks(&guiCallbacks);
}

/**
 *  Handle the login response
 */
void
do_loginrep(Packet packet)
{
   makeping(game2->time.current);
#ifdef DEBUG
   printf("+ login_rep -> ping is %d\n", getping());
#endif

   
   if( packet.infos.loginrep.accept != 1 ) //If login is refused
    {
      printf("login refused : %s.\n",packet.infos.loginrep.message);
      Net_disconnect();
      isConnected=0;
      serverstate=preGameState; //We hope that next time server will be to preGameState
      changeCallback(&trackerscreenCallbacks, &guiCallbacks);
      return;
    }

  isLogged=1;
  me = packet.which;
  slots[me].active=1;
#ifdef DEBUG
  printf("logged ( slot %d )...\n%s\n", me, packet.infos.loginrep.message);
#endif

  strcpy(server_message, packet.infos.loginrep.message);
  updateUsersListData();
  nbUsers++;
  switchCallbacks(&netPregameCallbacks);
}

/**
 *  Handle server infos
 */
void
do_serverinfo(Packet packet)
{
  int    lastserverstate = serverstate;
  Data  *data;

  //We get current number of players connected
  nbUsers = packet.infos.serverinfo.players;

  //check if we changed state
  if( serverstate != packet.infos.serverinfo.serverstate )
    {
      serverstate=packet.infos.serverinfo.serverstate;
      switch(serverstate)
	{
	case preGameState:
	  if( lastserverstate == gameState )
	    {
	      if( netscores.winner == -1 )
		{		  
		  switchCallbacks(&pauseCallbacks);
		}  else { //Game is finished
		  switchCallbacks(&scoresCallbacks);
		}
	    } else {
	      switchCallbacks(&netPregameCallbacks);
	    }
	  base = 0;
	  now = SDL_GetTicks();
	  break;
	case gameState:
	  //The game as started, we 'd be an observer
	  game2->players=nbUsers;

	  applyGameInfo();
#ifdef DEBUG
	  printf("time is %d\n", game2->time.current);
#endif

	  if( ! hasstarted )
	    {
	      hasstarted = 1;
	      timeout = SystemGetElapsedTime();
	    }
	  if( observerstate == 1 )
	    {
	      observerstate = 0; //next time we play!

	      //Setting player 1 to be observer
	      data = game->player[0].data;
	      data->speed = SPEED_CRASHED;
	      data->iposx = game2->startPositions[3];
	      data->iposy = game2->startPositions[4];
	      data->posx = data->iposx;
	      data->posy = data->iposy;
	      data->t = 0;
	      data->dir = game2->startPositions[5];
	      data->last_dir = data->dir;
	      data->speed = game->settings->current_speed;
	      
	      data->trail = data->trails;
	      
	      data->trail->sx = data->trail->ex = data->iposx;
	      data->trail->sy = data->trail->ey = data->iposy;
	      data->speed = SPEED_GONE;
	      data->trail_height = 0;
	      data->exp_radius = EXP_RADIUS_MAX;
	    }

	  game2->mode = GAME_NETWORK_PLAY;
	  ping[0] = slots[me].ping;
	  savedtime  = 0;
	 /*  game2->time.current   = SDL_GetTicks() - base; */
/* 	  game2->time.offset    = base; */
	  game2->time.offset    = now;
	  printf("######################## << TIME %u ( %u ) >> #####################\n", now, base);
	  //game2->time.current=0;

#ifdef DEBUG	  
	  printf("starting game at%d\n", SystemGetElapsedTime());
#endif
	  
	  switchCallbacks(&pauseCallbacks);
	  break;
	}
    }
}

/**
 *  Handle users info
 */
void
do_userinfo(Packet packet)
{
  char mesg[255];
  int  which = packet.infos.userinfo.which;
  //Just put in the slots...

  if( slots[which].active == -1 )
    {
#ifdef DEBUG
      printf("%s join\n", packet.infos.userinfo.nick);
#endif
      sprintf(mesg, "%s join\n", packet.infos.userinfo.nick);
      if( serverstate == preGameState )
	{ 
	  insert_wtext(pregame.pregametext, mesg, 8); 
	} else  {
	    consoleAddLine(mesg);
	  }
    }

  slots[which].active   = 1;
  strcpy(slots[which].name, packet.infos.userinfo.nick);
  slots[which].points   = 0;
  slots[which].color    = packet.infos.userinfo.color;
  slots[which].isMaster = packet.infos.userinfo.ismaster;
  
  if( which == me )
    {
      slots[me].player=0;
      if( slots[which].active == 1 )
	{
	  sprintf(mesg, "logged...\n");
#ifdef DEBUG
	  printf("logged...\n");
#endif
	  insert_wtext(pregame.pregametext, mesg, 3); 
	}

      //if we are the master activate controls
      if( slots[me].isMaster )
	{	  
	  activateControl( pregame.pregameControls, (Wptr)pregame.level );
	  activateControl( pregame.pregameControls, (Wptr)pregame.speed );
	  activateControl( pregame.pregameControls, (Wptr)pregame.size  );
	  activateControl( pregame.pregameControls, (Wptr)pregame.gameRule );
	  activateControl( pregame.pregameControls, (Wptr)pregame.gameType );
	  activateControl( pregame.pregameControls, (Wptr)pregame.start   );
	}
    } else {
      slots[which].player=(which==0)?me:which;
    }
  if( serverstate == preGameState )
    {
      updateUsersListData();
    }
}


/**
 *  Handle a chat packet
 */
void
do_chat( Packet packet )
{
  char mesg[255];

  if( packet.infos.chat.which == BROADCAST )
    {
      if( packet.which == me )
      {
#ifdef DEBUG
	printf("> %s\n", packet.infos.chat.mesg);
#endif
	sprintf(mesg, "> %s\n", packet.infos.chat.mesg);	  
      } else {
#ifdef DEBUG
	printf("%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
#endif
	sprintf(mesg, "%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      }
      if( serverstate == gameState )	
	consoleAddLine(mesg);
      else {
	insert_wtext(pregame.pregametext, mesg, chatcolor[packet.which]);
      }
    } else {
      printf("[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      sprintf(mesg, "[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      insert_wtext(pregame.pregametext, mesg, 75);  
    }
}

/**
 *  Server says that an AI player will play with us, just add it to our slots
 */
void
do_aiplayer(Packet packet)
{
  int i, j;

   for(j=0; j < packet.infos.action.which; ++j )
    {
      //Looking for a free slot
      for(i=0; i < MAX_PLAYERS; ++i)
	{
	  if( slots[i].active == 0 )
	    break;
	}

      //create slot
      slots[i].active = 2;
      slots[i].sock = NULL;
      slots[i].player=i;
    }
}

/**
 *  Handle an action
 */
void
do_action(Packet packet)
{
  char mesg[255];
  int  i;

  switch( packet.infos.action.type )
    {
    case JOIN:
      slots[packet.infos.action.which].active=-1;
      updateUsersListData();
      nbUsers++;
      break;
    case PART:
      slots[packet.infos.action.which].active=0;
      //slots[packet.infos.action.which].player=-1;
      sprintf(mesg, "%s part\n", slots[packet.infos.action.which].name);
#ifdef DEBUG
      printf("%s", mesg);
#endif

      if( serverstate == preGameState )
	{
	  insert_wtext(pregame.pregametext, mesg, 56);   
	  updateUsersListData();
	} else if ( serverstate == gameState )
	  {
	    consoleAddLine(mesg);
	  }
      nbUsers--;
      break;
    case CHGEGAMEMASTER:
      //for a reason ( certainly 'cause game master has leave ), Game Master has changed
      //new Game Master is packet.infos.action.which

      //Search for old game Master if he still connected and put it not GM
      for(i=0;i<MAX_PLAYERS; ++i)
	{
	  if( (slots[i].active == 1) && (slots[i].isMaster == 1) )
	    {
	      slots[i].isMaster = 0;
	    }
	}
      sprintf(mesg, "%s is new Game Master\n", slots[packet.infos.action.which].name);
#ifdef DEBUG
      printf("%s", mesg);
#endif

      if( serverstate == preGameState )
	{
	  insert_wtext(pregame.pregametext, mesg, 67);  
	  updateUsersListData();
	} else if ( serverstate == gameState )
	  {
	    consoleAddLine(mesg);
	  }
      slots[packet.infos.action.which].isMaster=1;
      break;
    case HASSTARTED:
      //deconnect
      serverstate=preGameState; //We hope that next time server will be to preGameState

      //Game has started go to netWaitCallbacks;
      /* if( serverstate == preGameState ) */
/* 	switchCallbacks(&netWaitCallbacks); */
      observerstate = 1;
      break;
    case PING: //Only in pregame
      //Need to reply to ping
      packet.which=me;
      Net_sendpacket(&packet, Net_getmainsock());
      break;
    case AIPLAYER:
      do_aiplayer(packet);
      updateUsersListData();
      break;
    }
}

/**
 *  Handle net rules. That is specific net rules.
 */
void
do_netrules(Packet packet)
{
  int gametype;

  packet.which     = SERVERID;
  packet.type      = NETRULES;
  netrulenbwins    = packet.infos.netrules.nbWins;
  netruletime      = packet.infos.netrules.time;
#ifdef DEBUG
  printf("Net rules : %d %d\n", packet.infos.netrules.nbWins, packet.infos.netrules.time);
#endif
  
  //Set Here the controls
  if( netruletime != 0 )    
    gametype = 1;
  else 
    gametype = 0;
  select_wpopmenu(pregame.gameType, gametype );
    
  switch( gametype )
    {
    case 0:
      select_wpopmenu(pregame.gameRule, netrulenbwins/5-1 );
      break;
    case 1:
      select_wpopmenu(pregame.gameRule, netruletime/5-1 );
      break;
    }
}

/**
 *  Handle game rules. And init game datas and struct.
 */
void
do_gamerules(Packet packet)
{
  int   i;

#ifdef DEBUG
  if( slots[me].isMaster == 1 )
    {
      makeping(game2->time.current);
      printf("ping answer is %d\n", getping());
    }

  printf("getting games rules...\n");
#endif


  //TODO: clean all this ugly code, and do a safe clean init function.
  game->settings->ai_player1  = ( slots[getWhich(0)].active >= 1 ) ? 0 : 2;
  game->settings->ai_player2  = ( slots[getWhich(1)].active >= 1 ) ? 0 : 2;
  game->settings->ai_player3  = ( slots[getWhich(2)].active >= 1 ) ? 0 : 2;
  game->settings->ai_player4  = ( slots[getWhich(3)].active >= 1 ) ? 0 : 2;

  
  initNetEventList( neteventlist );
  game->players               = packet.infos.gamerules.players;

  /** before doing initData check for active players */
  for(i=0; i<MAX_PLAYERS;  ++i)
    {
      game->player[i].ai->active = ( slots[getWhich(i)].active>=1 ) ? 0 : 2;
#ifdef DEBUG
      printf("activating player %d on slot %d : %d\n", i, getWhich(i), game->player[i].ai->active);
#endif
    }


  game->settings->current_speed = packet.infos.gamerules.speed;
  game2->rules.eraseCrashed   = packet.infos.gamerules.eraseCrashed;
  game->settings->game_speed  = packet.infos.gamerules.gamespeed;
  game->settings->grid_size   = packet.infos.gamerules.grid_size;
  game->settings->arena_size  = packet.infos.gamerules.arena_size;
  

  select_wpopmenu(pregame.speed, game->settings->game_speed );
  select_wpopmenu(pregame.size, game->settings->arena_size );
  set_wcheckbox(pregame.erase, game2->rules.eraseCrashed );


  initData();
  game2->time                 = packet.infos.gamerules.time;

#ifdef DEBUG
  printf("Get Server time: current is %d, offset is %d\n",game2->time.current, game2->time.offset);
  printf("initData with game speed = %d and so speed is %f\n", game->settings->game_speed, game->settings->current_speed );
#endif
}

/**
 *  Handle start pos ( server says us where we start )
 */
void
do_startpos(Packet packet)
{
  int  i, j;

 //Startpos
#ifdef DEBUG
  printf("+ %d players, getting start positions \n", game2->players);
#endif

  game2->startPositions = ( int * )malloc( 3 * MAX_PLAYERS * sizeof( int ) );

  for(i=0; i<MAX_PLAYERS; ++i)
    {
      j = getPlayer(i);
#ifdef DEBUG
      printf("get startpos server %d <-> client %d\n", i, j);
#endif
      if( j!= -1 && slots[j].active >= 1 )
	{
	  game2->startPositions[3*i+0]=packet.infos.startpos.startPos[3*j+0];
	  game2->startPositions[3*i+1]=packet.infos.startpos.startPos[3*j+1];
	  game2->startPositions[3*i+2]=packet.infos.startpos.startPos[3*j+2];
#ifdef DEBUG
	  printf("\n\npos %d %d %d for player %d\n\n\n", game2->startPositions[3*i+0],
		 game2->startPositions[3*i+1],
		 game2->startPositions[3*i+2], i);
#endif
	}
    }
}


/**
 *  Handle a packet Event
 */
void
do_event(Packet packet)
{
#ifdef DEBUG
  if( packet.infos.event.event.player == me )
    {
      if( packet.infos.event.event.type != EVENT_CRASH &&  packet.infos.event.event.type != EVENT_STOP)
	{
	  makeping(game2->time.current);
	}
    }
#endif

  //Add to poll
  addNetEvent(&packet.infos.event.event);

#ifdef DEBUG
  fprintf(stderr, "get event: %d %d %d %d %d ( current time %d )\n", packet.infos.event.event.type,
	  packet.infos.event.event.player, packet.infos.event.event.x,
	  packet.infos.event.event.y, packet.infos.event.event.timestamp, game2->time.current);
#endif
}


/** Answer to a synchronisation
 *  it is done at game start 
 */
void
do_synch( Packet packet )
{
  Packet rep;
  Uint32 last;
  float lag = 0.0f;
  
  rep.which                        = me;
  rep.type                         = SYNCH;
  rep.infos.synch.type             = packet.infos.synch.type ;
  rep.infos.synch.data.u.s         = packet.infos.synch.data.u.s;
  rep.infos.synch.data.u.c         = packet.infos.synch.data.u.c;

  last = (Uint32)now;
  now  = SDL_GetTicks();

  lag  = ( now - last ) / 2.0f ;

  printSynchPacket( packet );

  switch( packet.infos.synch.type ) {
  case 0:
    base = now - packet.infos.synch.data.u.s ;
    rep.infos.synch.data.u.c = now;
    Net_sendpacket(&rep, Net_getmainsock());  
    break;
  case 1:
    base -= lag;
    rep.infos.synch.data.u.c = SDL_GetTicks() - base;
    rep.infos.synch.data.s.s = ( (Sint32) ( now - base - packet.infos.synch.data.s.s ) ) / 2;
    Net_sendpacket(&rep, Net_getmainsock());  
    break;
  case 2:
    base -= packet.infos.synch.data.s.c ;
    rep.infos.synch.data.u.c = SDL_GetTicks() - base;
    rep.infos.synch.data.s.s = ( (Sint32) ( now - base - packet.infos.synch.data.s.s + lag ) ) / 2;
    Net_sendpacket(&rep, Net_getmainsock());  
    break;
  case 3:
    base -= packet.infos.synch.data.s.c ;
    printf("wait until %u\n", packet.infos.synch.data.u.s );
    while( SDL_GetTicks() - base < packet.infos.synch.data.u.s );
    printf("TIME = %u\n", SDL_GetTicks() - base );
    now = SDL_GetTicks() - base;
    break;
  }

}

/**
 *  Handle scores : it means game is finished too.
 */
void
do_score(Packet packet)
{
  int i=0;

  netscores.winner=packet.infos.score.winner;
  for(i=0;i<MAX_PLAYERS;++i)
    netscores.points[i]=packet.infos.score.points[i];

  //We change server state to gameState, so that game will ended
  serverstate = gameState;
}

/**
 *  Prepare game rules to show in pregame the settings that server has.
 */
void
do_gameset( Packet packet )
{
#ifdef DEBUG
  printf("getting gameset : %d %d %d\n", packet.infos.gameset.gamespeed,
	 packet.infos.gameset.eraseCrashed,
	 packet.infos.gameset.arena_size);
#endif

  game->settings->game_speed       = packet.infos.gameset.gamespeed;
  game2->rules.eraseCrashed        = packet.infos.gameset.eraseCrashed;
  game->settings->erase_crashed    = packet.infos.gameset.eraseCrashed;
  game->settings->arena_size       = packet.infos.gameset.arena_size;
  game->settings->ai_level         = packet.infos.gameset.ai_level;
  select_wpopmenu(pregame.speed, game->settings->game_speed );
  select_wpopmenu(pregame.size,  game->settings->arena_size );
  select_wpopmenu(pregame.level, game->settings->ai_level );
  set_wcheckbox(pregame.erase,   game2->rules.eraseCrashed );
}

/**
 *  get a ping packet. Only in pregame.
 */
void
do_playersping( Packet packet )
{
  int i=0;

  for(i=0; i < MAX_PLAYERS; ++i )
    {
      slots[i].ping = packet.infos.playersping.ping[i];
    }
  updateUsersListData();
}


/** Handle network traffic. */
void
do_preGameState( Packet packet )
{
  switch( packet.type )
    {
    case LOGINREP:
      do_loginrep(packet);
      break;
    case USERINFO:
      do_userinfo(packet);
      break;
    case SERVERINFO:
      do_serverinfo(packet);
      break;
    case CHAT:
      do_chat(packet);
      break;
    case GAMERULES:
      do_gamerules(packet);
      break;
    case STARTPOS:
      do_startpos(packet);
      break;
    case NETRULES:
      do_netrules(packet);
      break;
    case GAMESET:
      do_gameset(packet);
      break;
    case SCORE:
      do_score(packet); //if we are in pause, we can get score, and need to stop
      break;
    case PLAYERSPING:
      do_playersping(packet);
      break;
    case SYNCH:
      do_synch(packet);
      break;
    case ACTION:
      do_action(packet);
      break;
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the preGameState\n", packet.type);
      break;
    }
}

void
do_gameState( Packet packet )
{
  switch( packet.type )
    {
    case EVENT:
      do_event(packet);
      break;
    case SNAPSHOT:
      break;
    case SERVERINFO:
      do_serverinfo(packet);
      break;
    case USERINFO:
      do_userinfo(packet);
    case ACTION:
      do_action(packet);
      break;
    case SCORE:
      do_score(packet);
      break;
    case CHAT:
      do_chat(packet);
      break;
    case SYNCH:
      do_synch(packet);
      break;
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the gameState\n", packet.type);
      break;
    }
}

/**
 *  Handle datas from server
 */
void
handleServer()
{
  Packet packet;
  
  //Get the packet...
  if( Net_receivepacket(&packet, Net_getmainsock(), me, packet_type, slots) != 0 )
    {
      connectionLost();
      return;
    }
  if( packet_type == HEADER )
    {
      packet_type = slots[me].packet;
      return;
    }
  
  packet_type = slots[me].packet;

#ifdef DEBUG
  printf("recieve a packet. Type: %d. Serverstate: %d.\n", packet.type, serverstate);
#endif
  switch( serverstate )
    {
    case preGameState:
      do_preGameState(packet);
      break;
    case gameState:
      do_gameState(packet);
      break;
    default:
      fprintf(stderr, "Unknown server state %d\n", serverstate);
    } 
}  

int
getping()
{
  return ping[0]*.5+(ping[1]+ping[2]+ping[3]+ping[4])*.5*.25;
}

void
makeping(int time)
{
  if( savedtime == 0 )
    {
      savedtime = SystemGetElapsedTime();
    } else {
      ping[4]=ping[3];
      ping[3]=ping[2];
      ping[2]=ping[1];
      ping[1]=ping[0];
      ping[0] = SystemGetElapsedTime() - savedtime;
      savedtime=0;
    }
}

void
doPredictedTurn(int dir, int time)
{
  Predictedturn turn, new;

  new = (Predictedturn) malloc(sizeof(predictedturn));
  new->dir     = dir;
  new->time    = time;
  new->statut  = ASKED;
  new->next    = NULL;
  

  //insert in queue
  turn = turnlist->head;
  if( turn == NULL )
    {
      turnlist->head = new;
      return;
    }
  
  while( turn->next )
    { turn = turn->next; }

  turn->next = new;    
}


void
idleTurns(  )
{
  Predictedturn turn;
  GameEvent *e;

  turn = turnlist->head;

  if( turn != NULL )
    {
      //find first turn to predict
      while( turn != NULL  ) {
#ifdef DEBUG
	printf("turn status is %s ( %d / %d )\n", (turn->statut == PREDICTED)? "PREDICTED" : "ASKED", game2->time.current - turn->time, getping());
#endif
	if( turn->statut == ASKED )
	  break;
	turn=turn->next;
      }
      if( turn == NULL )	
	  return;

      if( (game2->time.current - turn->time) >= (getping()/2) )
	{
	  turn->statut = PREDICTED;
#ifdef DEBUG
	  printf("creating turn... at %d\n", game2->time.current);
	  printf("predicted pos is: %f, %f\n", 
		     game->player[0].data->posx,
		     game->player[0].data->posy);
#endif
	  e = (GameEvent*) malloc(sizeof(GameEvent));
	  switch(turn->dir) {
	  case TURN_LEFT: e->type = EVENT_TURN_LEFT; break;
	  case TURN_RIGHT: e->type = EVENT_TURN_RIGHT; break;
	  }
	  e->x = game->player[0].data->posx;
	  e->y = game->player[0].data->posy;
	  e->player = 0;
	  e->timestamp = game2->time.current;
	  processEvent(e);
	  turn->rdir=game->player[0].data->dir;
	}
    }
}

int
undoTurn(int x, int y, int time)
{
  int              ping=0;
  line             *old, *cur;
  Predictedturn    turn, aux, next;
  int              nbPredictedTurn;
  int              i;

#ifdef DEBUG
  printf("undoTurn pos x=%d, y=%d at time=%d\n", x, y, time);
  printf("current pos x=%d, y=%d at time=%d\n", game->player[0].data->iposx, game->player[0].data->iposy, game2->time.current);
#endif

  turn = turnlist->head;
  if( turn == NULL )
    return 0;

  nbPredictedTurn = get_size_predictedturn();

#ifdef DEBUG
  printf("nbPredictedTurn %d\n", nbPredictedTurn);
#endif

  //turn are ordered so it's 

  if( nbPredictedTurn == 0 )
    {     
      //free the prediction
      free_turn(turnlist->head);
      return 0;
    }

  //turn is our turn to undo
  old= game->player[0].data->trail-nbPredictedTurn;
  old->ex=x;
  old->ey=y;
 
	  
  cur=game->player[0].data->trail-nbPredictedTurn+1;
  cur->sx = x;
  cur->sy = y;

  if( nbPredictedTurn > 1 )
    {
      i=nbPredictedTurn+2;
      aux=turn;
      next=aux->next;
      //rearrange next trails
      while( cur != game->player[0].data->trail && next != NULL )
	{
	  old = cur;
	  //cur = game->player[0].data->trail-i--;
	  cur = old+1;

#ifdef DEBUG
	  printf("diff time is %d old->sx %d old->sy %d\n", next->time-aux->time,
		 old->sx, old->sy);
	  printf("change old->ex %d old->ey %d to", old->ex, old->ey);
#endif

	  old->ex=(next->time-aux->time)*game->player[0].data->speed*dirsX[ aux->rdir ]/100+old->sx;
	  old->ey=(next->time-aux->time)*game->player[0].data->speed*dirsY[ aux->rdir ]/100+old->sy;

#ifdef DEBUG
	  printf(" old->ex %d old->ey %d\n", old->ex, old->ey);
#endif
	  cur->sx=old->ex;
	  cur->sy=old->ey;
	  aux=next;
	  next=aux->next;
	}
    }

  //Changing position of cycle
  ping=getping();

#ifdef DEBUG
  printf("distance to change( %d - %d = %d ) is %f dirX %d, dirY %d\n",time+ping/2, time, (time+ping/2), (ping/2)*game->player[0].data->speed,dirsX[ game->player[0].data->dir],dirsY[ game->player[0].data->dir]);
#endif

  game->player[0].data->posx=(ping/2)*game->player[0].data->speed*dirsX[ game->player[0].data->dir]/100+cur->sx;
  
  game->player[0].data->posy=(ping/2)*game->player[0].data->speed*dirsY[ game->player[0].data->dir]/100+cur->sy;
	      
  game->player[0].data->iposx=game->player[0].data->posx;
  game->player[0].data->iposy=game->player[0].data->posy;

#ifdef DEBUG
  printf("new pos is: %f, %f\n",  
	 game->player[0].data->posx,
	 game->player[0].data->posy);
#endif

  //Free this turn!
  free_turn(turn);
  return 1;
}

void
correctTurn( GameEvent *e )
{
  float lag = game2->time.current - e->timestamp;
  float chge;
  line             *old, *cur;

  printf("current pos x=%d, y=%d at time=%d dir=%d\n", game->player[e->player].data->iposx, game->player[e->player].data->iposy, game2->time.current, game->player[e->player].data->dir);

  chge = lag * game->player[e->player].data->t;
  printf("player %d, lag = %f chge %f\n", e->player, lag, chge);


  old = game->player[e->player].data->trail-1;
  old->ex -=  chge * dirsX[ game->player[e->player].data->last_dir] ;
  old->ey -=  chge * dirsY[ game->player[e->player].data->last_dir];

  cur = game->player[e->player].data->trail;
  cur->sx = old->ex ;
  cur->sy = old->ey;
 

  
  game->player[e->player].data->posx = cur->sx + chge * dirsX[ game->player[e->player].data->dir ];
  
  game->player[e->player].data->posy = cur->sy + chge * dirsY[ game->player[e->player].data->dir ];
	       
  game->player[e->player].data->iposx = game->player[e->player].data->posx;
  game->player[e->player].data->iposy = game->player[e->player].data->posy;
  
  printf("----------->cur->sx %d cur->sy %d\n", cur->sx, cur->sy );


  printf("new pos is: %f, %f\n",  
	 game->player[e->player].data->posx,
	 game->player[e->player].data->posy);


}

void
createTurnList()
{
  turnlist = (TurnList*) malloc(sizeof(TurnList));
  turnlist->head = NULL;

}

void
initTurnList( )
{
  Predictedturn turn, next;

  //nsl must be allocate before...
  if( turnlist == NULL )
    return;

  turn = turnlist->head;

  if( turn == NULL )
    {
      return;
    }
  while( turn )
    { next = turn->next; free(turn); turn=next;printf("freeing turn...\n"); }
  turnlist->head=NULL;
}

void
free_turn(Predictedturn turn)
{
  Predictedturn cur, next;

  cur = turnlist->head;
  if( cur == NULL )
    return;

  if( turn == cur )
    {
      printf("free head\n");
      turnlist->head = turn->next;
      free(turn);
      return;
    }

  next = cur->next;
  
  //search turn
  while( next != NULL )
    {
      if( next == turn ) 
	break;
      cur = next;
      next = next->next;
    }
  cur->next = next->next;
  free(turn);
}

void
freeTurnList()
{
  initTurnList();
  free(turnlist);
  turnlist=NULL;
}

int
get_size_predictedturn()
{
  int           size = 0;
  Predictedturn turn = turnlist->head;

  if( turn == NULL )
    {
      fprintf(stderr, "Error when finding nb Predicted Turns\n");
      return 0;
    }

  while( turn )
    {
#ifdef DEBUG
      printf("turn status is %s\n", (turn->statut == PREDICTED)? "PREDICTED" : "ASKED");
#endif
      if( turn->statut == PREDICTED )
	size++; 
      turn=turn->next;
    }

  return size;
}
