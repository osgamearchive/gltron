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
  printf("logged...\n%s\n", packet.infos.loginrep.message);
  isLogged=1;
  me = packet.which;
  switchCallbacks(&netPregameCallbacks);
}

void
do_serverinfo(Packet packet)
{
  nbUsers = packet.infos.serverinfo.players;

  //check if we changed state
  if( serverstate != packet.infos.serverinfo.serverstate )
    {
      serverstate=packet.infos.serverinfo.serverstate;
      switch(serverstate)
	{
	case preGameState:
	  switchCallbacks(&netPregameCallbacks);
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
  int which = packet.infos.userinfo.which;
  //Just put in the slots...

  if( slots[which].active == -1 )
    {
      printf("%s join\n", packet.infos.userinfo.nick);
    }

  slots[which].active   = 1;
  strcpy(slots[which].name, packet.infos.userinfo.nick);
  slots[which].points   = 0;
  slots[which].color    = packet.infos.userinfo.color;
  slots[which].isMaster = packet.infos.userinfo.ismaster;

}

void
do_chat( Packet packet )
{
  if( packet.infos.chat.which == BROADCAST )
    {
      printf("%s > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
    } else {
      printf("[ %s ] > %s\n", slots[packet.which].name, packet.infos.chat.mesg);
    }
}

void
do_action(Packet packet)
{
  switch( packet.infos.action.type )
    {
    case JOIN:
      slots[packet.infos.action.which].active=-1;
      nbUsers++;
      break;
    case PART:
      slots[packet.infos.action.which].active=0;
      nbUsers--;
      break;
    }
}

void
do_netrules(Packet packet)
{
  Packet rep;

  rep.which     = SERVERID;
  rep.type      = NETRULES;
  netrulenbwins = rep.infos.netrules.nbWins;
  netruletime   = rep.infos.netrules.time;
}

void
do_gamerules(Packet packet)
{
  int   i, j;

  game->settings->ai_player1= ( slots[0].active ) ? 0 : 2;
  game->settings->ai_player2= ( slots[1].active ) ? 0 : 2;
  game->settings->ai_player3= ( slots[2].active ) ? 0 : 2;
  game->settings->ai_player4= ( slots[3].active ) ? 0 : 2;
  
  initData();
  
  game2->players              = packet.infos.gamerules.players;
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
