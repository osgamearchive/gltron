#include "gltron.h"



void
login(char *name)
{
  int           i;

  for(i=0;i<4;++i)
    {
      slots[i].active=0;
    }

  if( Net_SockisValid()  )
    {
      //Sending welcome to the server.
      Send_welcom(name, 1); //TODO: put gltron version here...
    }
  neteventlist = createNetEventList();
}

//get login answer
int
doLoginNetEvent(int accepted, int which, int len, int time)
{
  char message[512];
  int  i;

  //which must be serverid
  if( which != SERVERID )
    {

      fprintf(stderr, "Getting a packet from the server but has an id different from -1!!.\n");
      return corruptedPacket;
    }
  
  //Get a message from the server
  Recv_Buff(message, len); //Getting Welcome message from Server.

  if( ! accepted )
    { 
      fprintf(stderr, "Connection refused: %s", message);
      return loginNotAccepted;
    }


  //sprintf(message, "%s\n", message);
  //drawConsoleLines(messages, 0);

  //Get login info.
  Recv_login( &serverstate, &nbUsers );

  print_serverState(serverstate);

  //get our info, who we are, etc...
  which = Recv_who(slots);
  me = which;
  
  //are we the master?
  if( slots[which].isMaster )
    {
      fprintf(stderr, "You 're the game Master\n");
    }
      fprintf(stderr, "Cycle's color is: %d ( %s )\n", slots[which].color, slots[which].name);


  //Getting users list.
  fprintf(stderr, "%d palyers in the game\n", nbUsers);

  for(i=0; i<nbUsers; ++i)
    {
      which = Recv_who(slots);
    }
  return noErr;
}

//Get a magic event, for connection.
int
doMagicNetEvent()
{
  int err = noErr;
  err = Send_header(magic, SERVERID, 0, 0);
  //now we can logging in.
 login(game->settings->nickname); //logging in
  return err;
}

// S1 join
int
doJoinNetEvent()
{
  int  err = noErr;
  int  which = Recv_who(slots);

  fprintf(stderr, "%s join the game\n", slots[which].name);
  nbUsers++;
  return err;
}

//S& part
int
doPartNetEvent(int which)
{
  int err = noErr;
  fprintf(stderr, "%s part\n", slots[which].name);
  slots[which].active = 0;
  nbUsers--;
  return err;
}

//Server change state...
int
doChgeStateNetEvent()
{
  int err = noErr;
  int newState; 
  Data   *data;

  Recv_chgeState(&newState);
  fprintf(stderr, "Server has changed state:");

  print_serverState(newState);
  if( newState == preGameState )
    {
      Recv_netGameSettings(&netSettings);
      printNetGameSettings(&netSettings);
    }
  serverstate = newState;
  if( serverstate == gameState)
    {
      
      game->settings->ai_player1=( slots[0].active ) ? 0 : 2;
      game->settings->ai_player2=( slots[1].active ) ? 0 : 2;
      game->settings->ai_player3=( slots[2].active ) ? 0 : 2;
      game->settings->ai_player4=( slots[3].active ) ? 0 : 2;
      

      //starting the game...
      initData();
      if( Recv_rules() != noErr )//getting rules from server...
	fprintf(stderr, "got an error while reading rules\n");
      else
	fprintf(stderr, "rules set...\n");
      //game->players=game2->players;
      
      


      if( applyGameInfo() )//applying gamerules
	fprintf(stderr, "got an error while applying GameInfo\n");

      data = game->player[me].data;
      //data = (Data *)game2->startPositions[0];

      printf("init position with x: %d y: %d direction: %d\n", data->iposx,
	                                                        data->iposy,
	                                                        data->dir);

      fprintf(stderr, "starting the game\n");
      printf("grid size: %d\n", game->settings->grid_size);
      game2->mode = GAME_PLAY;
      switchCallbacks(&pauseCallbacks);
    }
  return err;
}

//Had a game event during playing...
int
doGameNetEvent(  )
{
  GameEvent   *e;
  int err = noErr;

  e = Recv_gameEvent();
  if( e == NULL )
    return noErr;
  addNetEvent(e);

  fprintf(stderr, "%d %d %d %d %d\n", e->type, e->player, 
  	 e->x, e->y, e->timestamp);

  switch( e->type )
  {
  case EVENT_TURN_LEFT:
    fprintf(stderr, "%s has turn left.\n", slots[e->player].name);
    break;
  case EVENT_TURN_RIGHT:
    fprintf(stderr,"%s has turn right.\n", slots[e->player].name);
    break;
  case EVENT_CRASH:
    fprintf(stderr,"%s has crashed.\n", slots[e->player].name);
    break;
  case EVENT_STOP:
    fprintf(stderr,"%s is the winner.\n", slots[e->player].name);
    break;
  default:
    fprintf(stderr,"Unknown game event.\n");
    break;
  }

  return err;
}

int
doChatNetEvent(int len, int which)
{
  int err=noErr;
  char mesg[255];
  
  Recv_chat(len, mesg);
  mesg[len]='\0';
  fprintf(stderr,"%s ( %d ) > %s\n", slots[which].name, which, mesg); 
  //sprintf(mesg,"%s ( %d ) > %s\n", slots[which].name, which, mesg); 
  //drawConsoleLines(mesg, 3);
  return err;
}


void
connectionLost()
{
  isConnected=0;
  Net_deconnect();
  switchCallbacks(&guiCallbacks);
}


/** Handle network traffic. */
void
handleServer()
{
  int         len;
  int         which;
  int         type;
  int         time;

  //get header to see what's about, who, when. 
  if( (Recv_header(&type, &which, &len, &time) )!= noErr )
    {
      //Net_deconnect();
      fprintf(stderr,"Connection lost\n");
      connectionLost();
      //exit(2);
      //Connection 's lost.
    } else {
      switch( type )
	{
	case loginAccept: //login accept
	  if( doLoginNetEvent(1, which, len, time) != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    } else {
	      switchCallbacks(&netPregameCallbacks);
	    }
	  break;
	case loginRefuse://login refused
	  if( doLoginNetEvent(0, which, len, time) != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	case magic:
	  if( doMagicNetEvent() != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	case joinPlayer:
	  if(doJoinNetEvent() != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	case leftPlayer:
	  if(doPartNetEvent(which) != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	case gameEvent:
	  if(doGameNetEvent() != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	case chgeState:
	  if(doChgeStateNetEvent() != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	case chat:
	  if(doChatNetEvent(len, which) != noErr )
	    {
	      //exit(2);
	      connectionLost();
	    }
	  break;
	}
    }
}
