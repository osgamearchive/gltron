#include "server_gltron.h"

#define VERSION 1
//TODO : manage version...
//server functions...
#define __DEBUG__
void
start_server()
{
  int        i;
  IPaddress  serverIP; //our IP.


  //Init the slots
  for(i=0;i<MAX_PLAYERS;++i)
    {
      slots[i].active = 0; //slots is inactive
      slots[i].sock   = 0;   //So no sock..
    }

  //Alloc Socket Set
  socketset = SDLNet_AllocSocketSet(MAX_PLAYERS+1);
  if( socketset == NULL )
    {
      fprintf(stderr, "Can't Allocate SocketSet: %s\n", SDLNet_GetError());
      exit(1);
    }

  //Getting IPaddress
  SDLNet_ResolveHost(&serverIP, NULL, SERVER_PORT);
  
  //Start listening to port.
  servsock = SDLNet_TCP_Open(&serverIP);
  if( servsock == NULL )
    {
      stop_server();
      fprintf(stderr, "Can't Open socket: %s\n", SDLNet_GetError());
      exit(1);
    }
  
  //add serversock to socketset
  SDLNet_TCP_AddSocket(socketset, servsock);
  
  sState   = waitState;
  nbUsers  = 0;
  //Server is started!
  printf("server listening to port %d\n", SERVER_PORT);
}

void
initNetwork()
{
  //init SDLNet
  if( SDLNet_Init() < 0 )
    {
      fprintf(stderr, "Can't init SDLNet: %s\n", SDLNet_GetError());
      exit(1);
    }


  //Put all ai_players to 'none'
  game->settings->ai_player1=2;
  game->settings->ai_player2=2;
  game->settings->ai_player3=2;
  game->settings->ai_player4=2;

  game2->mode = GAME_NETWORK_RECORD;

  atexit(stop_server);
}

void
stop_server()
{
  if( servsock != NULL )
    {
      SDLNet_TCP_Close(servsock);
      servsock=NULL;
    }

  if( socketset != NULL )
    {
      SDLNet_FreeSocketSet(socketset);
      socketset = NULL;
    }
}

void
handle_client(int which)
{
  char           message[255];
  int            i=0;
  int            len;
  pServRepHdr    serverRep;
  pLogin         loginRep;
  pWho           whoRep;
  pWelcom        welcomRep;
  char           *mesg;
  Data           *data;
  int            j=0;

  serverRep = (pServRepHdr) malloc( sizeof(tServRepHdr) );
  loginRep  = (pLogin) malloc( sizeof(tLogin) );
  whoRep    = (pWho) malloc( sizeof(tWho) );
  welcomRep = (pWelcom) malloc( sizeof(tWelcom));

  // printf("handle client %d\n", which);

  //Bon c parti, faut analyser le beans...
  len = SDLNet_TCP_Recv(slots[which].sock, serverRep, sizeof(tServRepHdr));
  if( len <= 0 ) //La connection est perdu
    {

      if( slots[which].active )
	{
	  slots[which].active=0;
	  nbUsers--;
	  //Il faut prévenir les autres qu'il a quitté.
	  serverRep->which = which;    //C le serveur qui parle
	  serverRep->type  = leftPlayer; //On refuse le login
	  serverRep->time  = 0;           //Pas utilisé
	  printf("%s part.\n", slots[which].name);

	  switch( which )
	    {
	    case 0:
	      game->settings->ai_player1 = 2;
	      break;
	    case 1:
	      game->settings->ai_player2 = 2;
	      break;
	    case 2:
	      game->settings->ai_player3 = 2;
	      break;
	    case 3:
	      game->settings->ai_player4 = 2;
	      break;
	    }
	  game->players++;
	  
	  //Ici il faut voir si c le master et en changer.
	  if( slots[which].isMaster ) {
	    //On doit mettre un autre master ici
	    slots[which].isMaster=0;
	    //On choisit le slot suivant s'il est dispo.
	    for( i=0; i<4; ++i)
	      {
		if( ! slots[i].active )
		  {
		    //printf("trouvé\n");
		    break;
		  }
	      }
	    //i est donc notre nouveau master.
	    slots[i].isMaster=1;
	  }
	  //Maintenant faut prévenir les autres.
	  //On balance un who sur l'utilsateur en question

	  
	  serverRep->len   = strlen(message);
	  for(i=0; i< 4; ++i)
	    {
	      if( slots[i].active )
		{
		  SDLNet_TCP_Send(slots[i].sock,(char *)serverRep,sizeof(tServRepHdr));
		}
	    }
	}
      SDLNet_TCP_DelSocket(socketset, slots[which].sock);
      SDLNet_TCP_Close(slots[which].sock);
      slots[which].sock=NULL;
    } else {
      switch( serverRep->type)
	{
	case chat:
	  //Allocation de la mémoire
	  mesg = ( char * ) malloc( serverRep->len );
	  
	  SDLNet_TCP_Recv(slots[which].sock, (char *)mesg, serverRep->len);

	  for(i=0; i<4; ++i)
	    {
	      if( slots[i].active )
		{
		  SDLNet_TCP_Send(slots[i].sock,(char *)serverRep, sizeof(tServRepHdr));
		  SDLNet_TCP_Send(slots[i].sock,(char *)mesg,serverRep->len);
		}
	    }

	  break;
	  
	case startGame:
	  printf("Starting the game\n");
	  initData();
	  
	  //On informe du changement d'état.
	  serverRep->which = SERVERID;
	  serverRep->type  = chgeState;		  
	  sState = gameState;
	  initGameStructures();
	  resetScores();
	  initData();
	  for(i=0; i<4; ++i)
	    {
	      if( slots[i].active )
		{
		  game2->players=nbUsers;
		  SDLNet_TCP_Send(slots[i].sock,(char *)serverRep, sizeof(tServRepHdr));
		  SDLNet_TCP_Send(slots[i].sock,(char *)&sState,sizeof(int));
		  printf("sending game rules to %d...\n", i);
		  //Then send nb players
		  SDLNet_TCP_Send(slots[i].sock,(char *)&game2->players,sizeof(int));
		  //send rules
		  len = game2->rules.speed * 1000;
		  SDLNet_TCP_Send(slots[i].sock,(char *)&len,sizeof(int));
		  SDLNet_TCP_Send(slots[i].sock,(char *)&game2->rules.eraseCrashed,sizeof(int));
		  //send startPositions
		  game2->startPositions=(int *) malloc(3 * game2->players * sizeof(int));
		  for(j=0; j<game2->players; ++j)
		    {
		      game2->startPositions[3*j+0]=game->player[j].data->iposx;
		      game2->startPositions[3*j+1]=game->player[j].data->iposy;
		      game2->startPositions[3*j+2]=game->player[j].data->dir;
		      printf("start position: %d %d %d\n", game->player[j].data->iposx,
			     game->player[j].data->iposy,
			     game->player[j].data->dir);
		    }

		  printf("game2->startPositions %d\n", *game2->startPositions);

		  SDLNet_TCP_Send(slots[i].sock,(char *)game2->startPositions, (3 * game2->players * sizeof(int)));
		  for(j=0; j<4; ++j)
		    {
		      if( slots[j].active )
			{
			  data = game->player[j].data;
			  
			  printf("init position with x: %d y: %d direction: %d : %d\n", data->iposx,
				 data->iposy,
				 data->dir, game2->startPositions[j]);
			}
		    }

		  //Send time t synchronize...
		  SDLNet_TCP_Send(slots[i].sock,(char *)&game2->time, sizeof(Time));
		  
		}
	    }
	  game->players = game2->players;
	  printf("starting game with %d players\n", game->players);
	  printf("grid size: %d\n", game->settings->grid_size);
	  break;

	case stopGame:
	  printf("Stoping the game\n");

	  //TODO: send scores...

	  //On informe du changement d'état.
	  serverRep->which = SERVERID;
	  serverRep->type  = chgeState;		  
	  sState = preGameState;
	  for(i=0; i<4; ++i)
	    {
	      if( slots[i].active )
		{
		  SDLNet_TCP_Send(slots[i].sock,(char *)serverRep, sizeof(tServRepHdr));
		  SDLNet_TCP_Send(slots[i].sock,(char *)&sState,sizeof(int));
		}
	    }
	  printf("done...\n");
	  game2->mode = GAME_NETWORK_RECORD;

	  break;

	case turnLeft:
	  printf("get turn left from %d\n", which);
	  createTurnEvent(which, TURN_LEFT);
	  break;

	case turnRight:
	  printf("get turn right from %d\n", which);
	  createTurnEvent(which, TURN_RIGHT);
	  break;

	case magic:
	  SDLNet_TCP_Recv(slots[which].sock, (char *)welcomRep, sizeof(tWelcom));
	  if( welcomRep->vers != VERSION )
	    {
	      printf("bad version: %d\n", welcomRep->vers);
	      //serverRep = (pServRepHdr ) malloc(sizeof(tServRepHdr));
	      serverRep->which = SERVERID;    //C le serveur qui parle
	      serverRep->type  = loginRefuse; //On refuse le login
	      serverRep->time  = 0;           //Pas utilisé
	      strcpy(message, "Version of gltron is not good.\n");
	      serverRep->len   = strlen(message);
	      SDLNet_TCP_Send(slots[which].sock,(char *)serverRep,sizeof(tServRepHdr));
	      SDLNet_TCP_Send(slots[which].sock,message,serverRep->len);	  
	    }
	  
	  if( nbUsers == 0 )
	    {
	      slots[which].isMaster = 1;
	    } else {
	      slots[which].isMaster = 0;
	    }
	  //On accepte la connection
	  strcpy(slots[which].name, welcomRep->name);
	  
	  //On répond en lui disant que c ok.
	  //serverRep = (pServRepHdr ) malloc(sizeof(tServRepHdr));
	  serverRep->which = SERVERID;    //C le serveur qui parle
	  serverRep->type  = loginAccept; //On accepte le login
	  serverRep->time  = 0;           //Pas utilisé
	  strcpy(message, "Welcome to gltron server.");
	  serverRep->len   = strlen(message);
	  SDLNet_TCP_Send(slots[which].sock,(char *)serverRep,sizeof(tServRepHdr));
	  SDLNet_TCP_Send(slots[which].sock,message,serverRep->len);
	  


	  //Ensuite on lui envoie les infos de login
	  //loginRep = (pLogin) malloc( sizeof(tLogin) );
	  loginRep->state    = sState;
	  loginRep->nbUsers  = nbUsers++;
	  SDLNet_TCP_Send(slots[which].sock,(char *)loginRep,sizeof(tLogin));
	  slots[which].active = 0;

	  //On lui envoie les infos persos
	  whoRep->which      = which;
	  whoRep->isMaster   = slots[which].isMaster;
	  whoRep->color      = 1;
	  whoRep->pingIt     = slots[which].pingIt;
	  strcpy(whoRep->name, slots[which].name);
	  SDLNet_TCP_Send(slots[which].sock,(char *)whoRep,sizeof(tWho));

	  //On envoi les infos des utilisateurs
	  //whoRep = (pWho ) malloc(sizeof(tWho));
	  for(i=0; i<4; ++i)
	    {
	      if( slots[i].active )
		{
		  whoRep->which      = i;
		  whoRep->pingIt     = slots[i].pingIt;
		  whoRep->color      = slots[i].color;
		  whoRep->isMaster   = slots[i].isMaster;
		  strcpy(whoRep->name, slots[i].name);
		  SDLNet_TCP_Send(slots[which].sock,(char *)whoRep,sizeof(tWho));
		}
	    }
	  
	  //On préviens les autres qu'un type vient d'entrer.
	  serverRep->which = SERVERID;    //C le serveur qui parle
	  serverRep->type  = joinPlayer;  //Un joueur vient d'arriver
	  whoRep->which=which;
	  strcpy(whoRep->name, slots[which].name);
	  whoRep->color = 0;
	  whoRep->pingIt = 0;
	  for(i=0;i<4;++i)
	    {
	      if( slots[i].active )
		{
       		  SDLNet_TCP_Send(slots[i].sock,(char *)serverRep,sizeof(tServRepHdr));
		  SDLNet_TCP_Send(slots[i].sock,(char *)whoRep,sizeof(tWho));
		}
	    }
	  slots[which].active = 1;
	  //On change l'état...
	  serverRep->which = SERVERID;    //C le serveur qui parle
	  serverRep->type  = chgeState;   //Un joueur vient d'arriver
	  SDLNet_TCP_Send(slots[which].sock,(char *)serverRep,sizeof(tServRepHdr));
	  sState = preGameState;
	  SDLNet_TCP_Send(slots[which].sock,(char *)&sState,sizeof(int));

	  printf("Changing state : %d\n", sState);
	  //On est en pre-game, on envoie les réglages de la partie.
	  SDLNet_TCP_Send(slots[which].sock,(char *)netSettings,sizeof(tnetGameSettings));

	}
    }
  free(serverRep);
  free(loginRep);
  free(whoRep);
  free(welcomRep); 
}

void
handle_connection()
{
  TCPsocket   newsock = NULL;
  int         which;
  pServRepHdr servRep = ( pServRepHdr ) malloc( sizeof(tServRepHdr));
  char        mesg[255];
  
  //Accept the new connection
  newsock = SDLNet_TCP_Accept( servsock );
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
  
  //If which == MAC_PLAYERS, there is no free slots...
  if( which >= MAX_PLAYERS )
    {
      //We must refuse the connection
      fprintf(stderr, "server is full, refuse login..\n");

      //Advice the client that the server is full...
      sprintf(mesg, "server is full, try again later...");
      servRep->which  = SERVERID;
      servRep->type   = loginRefuse;
      servRep->time   = 0; //TODO ...
      servRep->len    = strlen(mesg);
      //Send the infos
      SDLNet_TCP_Send(newsock, (void *) servRep, sizeof(tServRepHdr));
      SDLNet_TCP_Send(newsock, (void *) mesg, servRep->len);

      //We close the connection$
      SDLNet_TCP_Close(newsock);
    } else {
      //Server is not full...
      servRep->which  = SERVERID;
      servRep->type   = magic; //Send magic header
                               //to eval if login is approved
      servRep->time   = 0; //TODO ...
      servRep->len    = 0;
      SDLNet_TCP_Send(newsock, (void *) servRep, sizeof(tServRepHdr));

      //Savings slots
      slots[which].sock = newsock;
      slots[which].peer = *SDLNet_TCP_GetPeerAddress(newsock); //Get remote address
      slots[which].active = 1;
      SDLNet_TCP_AddSocket(socketset, slots[which].sock);
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
      //nbUsers++;
    }
  free(servRep);
}



void
handle_server()
{
  int i;
  
  // look for a net events
  SDLNet_CheckSockets(socketset, 1);
  
  // something appens, look what it was...
  if( SDLNet_SocketReady(servsock) )
  {
    printf("has a connection...\n");
    //this is a new connection...
    handle_connection();
  }
  
  //look what is said!
  for(i=0;i<MAX_PLAYERS;++i)
    {
      if( SDLNet_SocketReady(slots[i].sock) )
	{
	  //i said something, handle...
	  handle_client(i);
	}
    }
      

}

void
SendEvents(GameEvent *e)
{
  int i;
  pServRepHdr   serverRep;

  //Allocation de la mémoire
  serverRep = ( pServRepHdr ) malloc( sizeof(tServRepHdr) );

  //Initialisation
  serverRep->which = SERVERID;
  serverRep->type  = gameEvent;
  serverRep->len   = 0;
  serverRep->time  = 0;
  

  for( i=0; i <4; ++i)
    {
      if( slots[i].active )
	{
	  SDLNet_TCP_Send(slots[i].sock, (char*)serverRep, sizeof(tServRepHdr));
	  SDLNet_TCP_Send(slots[i].sock,(void *)e,sizeof(GameEvent));
	}
    }
  free(serverRep);
}
