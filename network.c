#include "gltron.h"



void
login(char *name)
{
  int           i;
  Packet        packet;

  for(i=0;i<4;++i)
    {
      slots[i].active=0;
    }

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
  switchCallbacks(&guiCallbacks);
}

void
do_loginrep(Packet packet)
{
  if( packet.infos.loginrep.accept != 1 )
    {
      printf("login refused : %s.\n",packet.infos.loginrep.message);
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
	      switchCallbacks(&pauseCallbacks);
	    } else {
	      switchCallbacks(&netPregameCallbacks);
	    }
	  break;
	case gameState:
	  //game->players = game2->players;
	  applyGameInfo();
	  game2->mode = GAME_PLAY;
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
      drawMessage(mesg);
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
  switch( packet.infos.action.type )
    {
    case JOIN:
      slots[packet.infos.action.which].active=-1;
      nbUsers++;
      break;
    case PART:
      slots[packet.infos.action.which].active=0;
      sprintf(mesg, "%s part\n", slots[packet.infos.action.which].name);
      drawMessage(mesg);
      nbUsers--;
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
  int   i, j;


  //TODO: clean all this ugly code, and do a safe clean init function.
  game->settings->ai_player1= ( slots[0].active ) ? 0 : 2;
  game->settings->ai_player2= ( slots[1].active ) ? 0 : 2;
  game->settings->ai_player3= ( slots[2].active ) ? 0 : 2;
  game->settings->ai_player4= ( slots[3].active ) ? 0 : 2;
  
  initNetEventList( neteventlist );
  game->players               = packet.infos.gamerules.players;

  /** before doing initData check for active players */
  for(i=0; i<MAX_PLAYERS;  ++i)
    {
      game->player[i].ai->active = ( slots[i].active ) ? 0 : 2;
    }

  initData();

  //game->players=game2->players;
  game2->rules.speed          = packet.infos.gamerules.speed;
  game2->rules.eraseCrashed   = packet.infos.gamerules.eraseCrashed;
  game->settings->game_speed  = packet.infos.gamerules.gamespeed;
  game->settings->grid_size   = packet.infos.gamerules.grid_size;
  game->settings->arena_size  = packet.infos.gamerules.arena_size;
  game2->time                 = packet.infos.gamerules.time;
  
  //Startpos
  printf("+ %d players, getting start positions \n", game2->players);
  game2->startPositions = ( int *)malloc(3*game2->players *sizeof(int));
  for(i=0; i<game2->players; ++i)
    {
      j = getPlayer(i);
      game2->startPositions[3*i+0]=packet.infos.gamerules.startPos[3*j+0];
      game2->startPositions[3*i+1]=packet.infos.gamerules.startPos[3*j+1];
      game2->startPositions[3*i+2]=packet.infos.gamerules.startPos[3*j+2];
    }
}

void
do_event(Packet packet)
{
  addNetEvent(&packet.infos.event.event);
  fprintf(stderr, "get event: %d %d %d %d %d\n", packet.infos.event.event.type,
	  packet.infos.event.event.player, packet.infos.event.event.x,
	  packet.infos.event.event.y, packet.infos.event.event.timestamp);
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
    case NETRULES:
      do_netrules(packet);
      break;
    case SCORE:
      break;
    case ACTION: //Is for PART
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
    default:
      fprintf(stderr, "Received a packet with a type %d that not be allowed in the preGameState\n", packet.type);
      break;
    }
}


void
handleServer()
{
  Packet packet;
  
  //Get the packet...
  if( Net_receivepacket(&packet, Net_getmainsock()) != 0 )
    {
      //Connection perdu
      connectionLost();
    }
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
