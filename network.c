#include "gltron.h"

static int ping = 0;
static int savedtime = 0; 

void
login(char *name)
{
  int           i;
  Packet        packet;

  for(i=0;i<4;++i)
    {
      slots[i].active=0;
      slots[i].packet=HEADER;    
      slots[i].player = -1;    
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
  nbUsers++;
  switchCallbacks(&netPregameCallbacks);
}

void
do_serverinfo(Packet packet)
{
  int lastserverstate = serverstate;
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
	  applyGameInfo();
	  printf("time is %d\n", game2->time.current);
	  game2->mode = GAME_NETWORK_PLAY;
	  ping = 0;
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
	  drawMessage(mesg);
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
      drawMessage(mesg);
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
      printf("%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      sprintf(mesg, "%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      drawChat(mesg);
    } else {
      printf("[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      sprintf(mesg, "[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
      drawChat(mesg);
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
	  drawMessage(mesg);
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
      slots[packet.infos.action.which].isMaster=1;
      break;
    case HASSTARTED:
      //deconnect
      serverstate=preGameState; //We hope that next time server will be to preGameState

      //Game has started go to netWaitCallbacks;
      if( serverstate == preGameState )
	switchCallbacks(&netWaitCallbacks);
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
  printf("Get Server time: current is %d, offset is %d\n",game2->time.current, game2->time.offset);
  printf("initData with game speed = %d and so speed is %f\n", game->settings->game_speed, game->settings->current_speed );
  initData();
  game2->time                 = packet.infos.gamerules.time;
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
	  printf("\n\npos %d %d %d\n\n\n", game2->startPositions[3*i+0],
		 game2->startPositions[3*i+1],
		 game2->startPositions[3*i+2]);
	}
    }
}

void
do_event(Packet packet)
{
  if( packet.infos.event.event.player == getPlayer(me) )
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
  netscores.winner=packet.infos.score.winner;
  memcpy(netscores.points, packet.infos.score.points, 4*MAX_PLAYERS);
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
    case SCORE:
      break;
    case ACTION: //Is for PART
      do_action(packet);	/*  */
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
    case ACTION:
      do_action(packet);
      break;
    case SCORE:
      do_score(packet);
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
  
  //Get the packet...
  if( Net_receivepacket(&packet, Net_getmainsock(), me, packet_type) != 0 )
    {
      //Connection perdu
      connectionLost();
      return;
    }
  if( packet_type == HEADER )
    {
      packet_type = slots[me].packet;
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
}  


int
getping()
{
  return ping;
}

void
makeping(int time)
{
  if( savedtime == 0 )
    {
      savedtime = time;
    } else {
      ping = savedtime - time;
      savedtime=0;
    }
}
