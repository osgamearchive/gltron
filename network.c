#include "gltron.h"

static Uint32 ping = 0;
static Uint32 savedtime = 0; 
static int    observerstate = 0;
static int    nbpings = 1;

enum
  {
    ASKED,         //the turn has been asked to the server
    PREDICTED      //the client has made the turn ( perdiction )    
  };
typedef struct Predictedturn *Predictedturn;
typedef struct Predictedturn {
  int            dir;
  int            time;
  int            statut;
  Predictedturn  next;
} predictedturn;

typedef struct TurnList {
  Predictedturn  head;
} TurnList;

TurnList *turnlist;

void free_turn(Predictedturn turn);

void
login(char *name)
{
  int           i=0;
  Packet        packet;
  
  
  makeping(i);
 
  for(i=0;i<4;++i)
    {
      slots[i].active  = 0;
      slots[i].packet  = HEADER;    
      slots[i].player  = -1;   
      slots[i].ping    = 0;
    }

  packet_type = HEADER;

  if( Net_getmainsock()!=NULL  )
    {
      //Sending welcome to the server.
      //TODO: put gltron version here...
      packet.which=me;
      packet.type=LOGIN;
      strcpy(packet.infos.login.version, VERSION);
      strcpy(packet.infos.login.nick, name);
      printf("+ send login\n");
      Net_sendpacket(&packet, Net_getmainsock());
      printf("- send login\n");
    }
  neteventlist = createNetEventList();
  hasstarted = 0;
}

void
connectionLost()
{
  fprintf(stderr, "connection lost\n");
  isConnected=0;
  isLogged=0;
  Net_disconnect();
  serverstate=preGameState; //We hope that next time server will be to preGameState
  

  switchCallbacks(&guiCallbacks);
}

void
do_loginrep(Packet packet)
{
   makeping(game2->time.current);
   printf("+ login_rep -> ping is %d\n", getping());
  if( packet.infos.loginrep.accept != 1 )
    {
      printf("login refused : %s.\n",packet.infos.loginrep.message);
      Net_disconnect();
      isConnected=0;
      serverstate=preGameState; //We hope that next time server will be to preGameState
      restoreCallbacks();
      return;
    }
  isLogged=1;
  me = packet.which;
  slots[me].active=1;
  printf("logged ( slot %d )...\n%s\n", me, packet.infos.loginrep.message);
  //sprintf(server_message, "%s\n", packet.infos.loginrep.message);
  strcpy(server_message, packet.infos.loginrep.message);
  nbUsers++;
  switchCallbacks(&netPregameCallbacks);
}

void
do_serverinfo(Packet packet)
{
  int lastserverstate = serverstate;
  //Packet rep;
  Data *data;

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
	  break;
	case gameState:
	  //game->players = game2->players;
	  game2->players=nbUsers;
	  applyGameInfo();
	  printf("time is %d\n", game2->time.current);
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
/* 	  if( slots[me].isMaster == 1 ) */
/* 	    { */
/* 	      printf("->  send confirmation... %d\n", ping); */
/* 	      rep.which=me; */
/* 	      rep.type=ACTION; */
/* 	      rep.infos.action.type=CONFSTART; */
/* 	      rep.infos.action.which=ping; */
/* 	       time server get ping is ping/2 and it needs to get */
/* 		  ping/2, so 2*ping/2=ping                           */
/* 	      Net_sendpacket(&rep, Net_getmainsock()); */
/* 	    } // I used that for synchronization. */
      //ping       = 0;
	  ping = slots[me].ping;
	  nbpings = 1;
	  savedtime  = 0;
	  game2->time.current=0;
	  
	  printf("starting game at%d\n", SystemGetElapsedTime());
	  
	  switchCallbacks(&pauseCallbacks);
	  break;
	}
    }
}

void
do_userinfo(Packet packet)
{
  char mesg[255];
  int  which = packet.infos.userinfo.which;
  //Just put in the slots...

  if( slots[which].active == -1 )
    {
      printf("%s join\n", packet.infos.userinfo.nick);
      sprintf(mesg, "%s join\n", packet.infos.userinfo.nick);
      //printf("%s", mesg);
      if( serverstate == preGameState )
	{
	  //drawMessage(mesg);	  
	  insert_wtext(pregametext, mesg, 2);
	} else if ( serverstate == gameState )
	  {
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
      printf("logged...\n");
      insert_wtext(pregametext, mesg, 2);
    }
    } else {
      slots[which].player=(which==0)?me:which;
    }
}

void
do_chat( Packet packet )
{
  char mesg[255];

  if( packet.infos.chat.which == BROADCAST )
    {
      if( packet.which == me )
      {
	printf("> %s\n", packet.infos.chat.mesg);
	sprintf(mesg, "> %s\n", packet.infos.chat.mesg);	  
      } else {
	printf("%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
	sprintf(mesg, "%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      }
      if( serverstate == gameState )	
	consoleAddLine(mesg);
      else
	insert_wtext(pregametext, mesg, 4);
	  
	  //drawChat(mesg);
    } else {
      printf("[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      sprintf(mesg, "[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      insert_wtext(pregametext, mesg, 3);
      //drawChat(mesg);
    }
}

void
do_action(Packet packet)
{
  char mesg[255];
  int  i;

  switch( packet.infos.action.type )
    {
    case JOIN:
      slots[packet.infos.action.which].active=-1;
      nbUsers++;
      break;
    case PART:
      slots[packet.infos.action.which].active=0;
      //slots[packet.infos.action.which].player=-1;
      sprintf(mesg, "%s part\n", slots[packet.infos.action.which].name);
      printf("%s", mesg);

      if( serverstate == preGameState )
	{
	  insert_wtext(pregametext, mesg, 2);
	  //drawMessage(mesg);
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
      printf("%s", mesg);

      if( serverstate == preGameState )
	{
	  insert_wtext(pregametext, mesg, 2);
	  //drawMessage(mesg);
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
    case PING:
      //Need to reply to ping
      packet.which=me;
      Net_sendpacket(&packet, Net_getmainsock());
      break;
    }
}

void
do_netrules(Packet packet)
{

  packet.which     = SERVERID;
  packet.type      = NETRULES;
  netrulenbwins    = packet.infos.netrules.nbWins;
  netruletime      = packet.infos.netrules.time;
  printf("Net rules : %d %d\n", packet.infos.netrules.nbWins, packet.infos.netrules.time);
  
}

void
do_gamerules(Packet packet)
{
  int   i;

  if( slots[me].isMaster == 1 )
    {
      makeping(game2->time.current);
      printf("ping answer is%d\n", getping());
    }
  printf("getting games rules...\n");
  //TODO: clean all this ugly code, and do a safe clean init function.
  game->settings->ai_player1  = ( slots[getWhich(0)].active == 1 ) ? 0 : 2;
  game->settings->ai_player2  = ( slots[getWhich(1)].active == 1 ) ? 0 : 2;
  game->settings->ai_player3  = ( slots[getWhich(2)].active == 1 ) ? 0 : 2;
  game->settings->ai_player4  = ( slots[getWhich(3)].active == 1 ) ? 0 : 2;

  
  initNetEventList( neteventlist );
  game->players               = packet.infos.gamerules.players;

  /** before doing initData check for active players */
  for(i=0; i<MAX_PLAYERS;  ++i)
    {
      game->player[i].ai->active = ( slots[getWhich(i)].active==1 ) ? 0 : 2;
      printf("activating player %d on slot %d : %d\n", i, getWhich(i), game->player[i].ai->active);
    }


  //game->players=game2->players;
  //game2->rules.speed          = packet.infos.gamerules.speed;
  game->settings->current_speed = packet.infos.gamerules.speed;

  game2->rules.eraseCrashed   = packet.infos.gamerules.eraseCrashed;
  
  //game->settings->eraseCrashed = packet.infos.gamerules.eraseCrashed;
  game->settings->game_speed  = packet.infos.gamerules.gamespeed;
  game->settings->grid_size   = packet.infos.gamerules.grid_size;
  game->settings->arena_size  = packet.infos.gamerules.arena_size;
  initData();
  game2->time                 = packet.infos.gamerules.time;
  printf("Get Server time: current is %d, offset is %d\n",game2->time.current, game2->time.offset);
  printf("initData with game speed = %d and so speed is %f\n", game->settings->game_speed, game->settings->current_speed );
}

void
do_startpos(Packet packet)
{
  int  i, j;

 //Startpos
  printf("+ %d players, getting start positions \n", game2->players);
  game2->startPositions = ( int *)malloc(3*MAX_PLAYERS *sizeof(int));
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      j = getPlayer(i);
      printf("get startpos server %d <-> client %d\n", i, j);
      if( j!= -1 && slots[j].active == 1 )
	{
	  game2->startPositions[3*i+0]=packet.infos.startpos.startPos[3*j+0];
	  game2->startPositions[3*i+1]=packet.infos.startpos.startPos[3*j+1];
	  game2->startPositions[3*i+2]=packet.infos.startpos.startPos[3*j+2];
	  printf("\n\npos %d %d %d for player %d\n\n\n", game2->startPositions[3*i+0],
		 game2->startPositions[3*i+1],
		 game2->startPositions[3*i+2], i);
	}
    }
}

void
do_event(Packet packet)
{
  if( packet.infos.event.event.player == me )
    {
      if( packet.infos.event.event.type != EVENT_CRASH &&  packet.infos.event.event.type != EVENT_STOP)
	{
	  makeping(game2->time.current);
	}
    }
  addNetEvent(&packet.infos.event.event);
  fprintf(stderr, "get event: %d %d %d %d %d ( current time %d )\n", packet.infos.event.event.type,
	  packet.infos.event.event.player, packet.infos.event.event.x,
	  packet.infos.event.event.y, packet.infos.event.event.timestamp, game2->time.current);
}

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

void
do_gameset( Packet packet )
{
  printf("getting gameset : %d %d %d\n", packet.infos.gameset.gamespeed,
	 packet.infos.gameset.eraseCrashed,
	 packet.infos.gameset.arena_size);
  game->settings->game_speed  = packet.infos.gameset.gamespeed;
  game2->rules.eraseCrashed   = packet.infos.gameset.eraseCrashed;
  game->settings->erase_crashed   = packet.infos.gameset.eraseCrashed;
  game->settings->arena_size  = packet.infos.gameset.arena_size;
}

void
do_playersping( Packet packet )
{
  int i=0;

  for(i=0; i < MAX_PLAYERS; ++i )
    {
      slots[i].ping = packet.infos.playersping.ping[i];
    }
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
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the gameState\n", packet.type);
      break;
    }
}


void
handleServer()
{
  Packet packet;
  
  //makeping(0);
  //Get the packet...
  if( Net_receivepacket(&packet, Net_getmainsock(), me, packet_type, slots) != 0 )
    {
      //Connection perdu
      connectionLost();
      return;
    }
  if( packet_type == HEADER )
    {
      packet_type = slots[me].packet;
      printf("recieve a header at %d\n", game2->time.current);
      //makeping(1);
      //printf("## handleserver took %d ms\n", getping());
      return;
    }
  
  packet_type = slots[me].packet;

  printf("recieve a packet. Type: %d. Serverstate: %d.\n", packet.type, serverstate);
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
  //makeping(1);
  //printf("## handleserver took %d ms\n", getping());
}  

#ifdef USEUDP
void
handleUDP()
{
  Packet **packets;
  Packet packet;

  packets = ( Packet ** ) malloc(sizeof(Packet *));

  //Get the packet...
  if( Net_receiveudppacket(packets, Net_getudpsock()) != 0 )
    {
      //Connection perdu
      connectionLost();
      return;
    }
  packet = *(packets[0]);

  printf("recieve a udp packet. Type: %d. Serverstate: %d.\n", packet.type, serverstate);
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
#endif

int
getping()
{
  return ping/nbpings;
}

void
makeping(int time)
{
  if( savedtime == 0 )
    {
      savedtime = SystemGetElapsedTime();
      //ping = 0;
    } else {
      ping += SystemGetElapsedTime() - savedtime;
      ++nbpings;
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
	printf("turn status is %s ( %d / %d )\n", (turn->statut == PREDICTED)? "PREDICTED" : "ASKED", game2->time.current - turn->time, ping);
	if( turn->statut == ASKED )
	  break;
	turn=turn->next;
      }
      if( turn == NULL )	
	  return;

      if( (game2->time.current - turn->time) >= ((short)ping/(2*nbpings)) )
	{
	  turn->statut = PREDICTED;
	  printf("creating turn... at %d\n", game2->time.current);
	  printf("predicted pos is: %f, %f\n", 
		     game->player[0].data->posx,
		     game->player[0].data->posy);
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
	}
    }
}

int
undoTurn(int x, int y, int time)
{
  int              ping=0;
  line             *old, *cur;
  Predictedturn    turn;
  int              nbPredictedTurn;

  printf("undoTurn pos x=%d, y=%d at time=%d\n", x, y, time);

  turn = turnlist->head;
  if( turn == NULL )
    return 0;

  //search for turn to undo turn = turnlist->head;
  //we need to get the last PREDICTED ( oldest one )
  while( turn!= NULL && turn->statut != PREDICTED ) { turn=turn->next; }

  nbPredictedTurn = get_size_predictedturn();
  printf("nbPredictedTurn %d\n", nbPredictedTurn);

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

  //Changing position of cycle
  ping=getping();

printf("distance to change( %d - %d = %d ) is %f dirX %d, dirY %d\n",time+ping/2, time, (time+ping/2), (ping/2)*game->player[0].data->speed,dirsX[ game->player[0].data->dir],dirsY[ game->player[0].data->dir]);

  game->player[0].data->posx=(ping/2)*game->player[0].data->speed*dirsX[ game->player[0].data->dir]/100+x;
  
  game->player[0].data->posy=(ping/2)*game->player[0].data->speed*dirsY[ game->player[0].data->dir]/100+y;
	      
  game->player[0].data->iposx=game->player[0].data->posx;
  game->player[0].data->iposy=game->player[0].data->posy;

  printf("new pos is: %f, %f\n",  
	 game->player[0].data->posx,
	 game->player[0].data->posy);

  //Free this turn!
  free_turn(turn);
  return 1;
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
      printf("turn status is %s\n", (turn->statut == PREDICTED)? "PREDICTED" : "ASKED");
      if( turn->statut == PREDICTED )
	size++; 
      turn=turn->next;
    }

  return size;
}
