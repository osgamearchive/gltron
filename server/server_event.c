#include "server_gltron.h"

int processEvent(GameEvent* e) {
  int      value = 0;
  Data     *data;
  Packet   rep;
  /**
  int  i;
  int  sState = preGameState;
  pServRepHdr    serverRep;
  */

  //if(game2->mode == GAME_PLAY) {
    writeEvent(e);
    //}
  switch(e->type) {
  case EVENT_TURN_LEFT:
    data = game->player[e->player].data;
    if(data->speed > 0) {
      data->iposx = e->x;
      data->iposy = e->y;
      data->turn = TURN_LEFT;
      doTurn(data, e->timestamp);
    }
    break;
  case EVENT_TURN_RIGHT:
    data = game->player[e->player].data;
    if(data->speed > 0) {
      data->iposx = e->x;
      data->iposy = e->y;
      data->turn = TURN_RIGHT;
      doTurn(data, e->timestamp);
    }
    break;
  case EVENT_CRASH: 
    data = game->player[e->player].data;
    data->posx = data->iposx = e->x;
    data->posy = data->iposy = e->y;
    sprintf(messages, "player %d crashed", e->player + 1);
    fprintf(stderr, "%s\n", messages);
    crashPlayer(e->player);
    break;
  case EVENT_STOP:
    fprintf(stderr, "game stopped\n");
    if(game2->mode == GAME_SINGLE_RECORD) {
      stopRecording();
      game2->mode = GAME_SINGLE;
    } else if(game2->mode == GAME_PLAY) {
      stopPlaying();
      game2->mode = GAME_SINGLE;
    }
    game->winner = e->player;
    sprintf(messages, "winner: %d", game->winner + 1);
    printf("%s\n", messages);

    game->pauseflag = PAUSE_GAME_FINISHED;
    //start a new game...
    game2->mode = GAME_NETWORK_RECORD;
    
    //go to pregame state...
    rep.which=SERVERID;
    rep.type=SERVERINFO;
    rep.infos.serverinfo.serverstate=preGameState;
    
    /*
    serverRep = (pServRepHdr) malloc( sizeof(tServRepHdr) );
    
    serverRep->which = SERVERID;
    serverRep->type  = chgeState;	
    for(i=0; i<4; ++i)
      {
	if( slots[i].active )
	  {
	    SDLNet_TCP_Send(slots[i].sock,(char *)serverRep, sizeof(tServRepHdr));
	    SDLNet_TCP_Send(slots[i].sock,(char *)&sState,sizeof(int));
	  }
      }
    free(serverRep);
    */
    value = 1;
    break;
  }
  free(e);
  return value;
}

list* doMovement(int mode, int dt) {
  int i;
  float fs;
  Data *data;
  list *l = NULL;
  GameEvent *e;

  for(i = 0; i < game->players; i++) {
    data = game->player[i].data;
    if(data->speed > 0) { /* still alive */

#define FREQ 1200
#define FACTOR 0.09
      fs = 1.0 - FACTOR + FACTOR * 
	cos(i * M_PI / 4.0 + 
	    (float)(game2->time.current % FREQ) * 2.0 * M_PI / (float)FREQ);
#undef FREQ
#undef FACTOR

      data->t += dt / 100.0 * data->speed * fs;
      while(data->t >= 1) {
	moveStep(data);
	data->t--;
	if(getCol(data->iposx, data->iposy) && mode) {
	  e = (GameEvent*) malloc(sizeof(GameEvent));
	  e->type = EVENT_CRASH;
	  e->player = i;
	  e->x = data->iposx;
	  e->y = data->iposy;
	  e->timestamp = game2->time.current;
	  addList(&l, e);
	  break;
	} else {
	  writePosition(i);
	}
      }
      data->posx = data->iposx + data->t * dirsX[data->dir];
      data->posy = data->iposy + data->t * dirsY[data->dir];
    } else { /* already crashed */
      if(game2->rules.eraseCrashed == 1 && data->trail_height > 0)
	data->trail_height -= (float)(dt * TRAIL_HEIGHT) / 1000;
      if(data->exp_radius < EXP_RADIUS_MAX)
	data->exp_radius += (float)dt * EXP_RADIUS_DELTA;
      else if (data->speed == SPEED_CRASHED) {
	int winner = -1;

	data->speed = SPEED_GONE;
	game->running--;
	if(game->running <= 1) { /* all dead, find survivor */
	  int i, maxSpeed = SPEED_GONE;
	  /* create winner event */
	  for(i = 0; i < game->players; i++) {
	    if(game->player[i].data->speed >= maxSpeed) {
	      winner = i;
	      maxSpeed = game->player[i].data->speed;
	    }
	  }
	  if(mode) {
	    e = (GameEvent*) malloc(sizeof(GameEvent));
	    e->type = EVENT_STOP;
	    e->player = winner;
	    e->timestamp = game2->time.current;
	    e->x = 0; e->y = 0;
	    addList(&l, e);
	    /* a stop event is the last event that happens */
	    return l;
	  }
	}
      }
    }      
  }
  return l;
}

 
void idleGame( void ) {
  list *l;
  list *p;
  int i;
  int dt;
  int t;

  handle_server();
  //if((updateTime() == 0) && ( game2->mode != GAME_NETWORK_RECORD )) return;
  if(updateTime() == 0) return;
  switch(game2->mode) {
  case GAME_NETWORK_RECORD:
    break;
    /* fall through */
  case GAME_SINGLE:
  case GAME_SINGLE_RECORD:
    /* check for fast finish */
    if(game->settings->fast_finish == 1) {
      int factor = 4;
      for(i = 0; i < game->players; i++) {
	if(game->player[i].ai->active != 1 &&
	   game->player[i].data->exp_radius < EXP_RADIUS_MAX)
	  factor = 1;
      }
      dt = game2->time.dt * factor;
    } else  
      dt = game2->time.dt;

    while(dt > 0) {
      if(dt > PHYSICS_RATE) t = PHYSICS_RATE;
      else t = dt;

      /* run AI */
      for(i = 0; i < game->players; i++)
	if(game->player[i].ai != NULL)
	  if(game->player[i].ai->active == 1 &&
	     game->player[i].data->speed > 0) {
	    if(game->settings->ai_level < 2)
	      doComputer(i, 0);
	    else 
	      doComputer2(i, 0);
	  }

      /* process any outstanding events (turns, etc) */
      for(p = &(game2->events); p->next != NULL; p = p->next) {
	if(processEvent((GameEvent*) p->data)) return;
      }

      /* free events */
      p = game2->events.next;
      while(p != NULL) {
	l = p;
	p = p->next;
	free(l);
      }
      game2->events.next = NULL;

      l = doMovement(1, t); /* this can generate new events */
      if(l != NULL) {
	for(p = l; p->next != NULL; p = p->next) {
	  if(processEvent((GameEvent*) p->data));
	}

      }
      /* free list  */
      p = l;
      while(p != NULL) {
	l = p;
	p = p->next;
	free(l);
      }
      dt -= PHYSICS_RATE;
    }
    break;
  case GAME_PLAY_NETWORK:
    //handle_server();
    /* fall through to GAME_PLAY */
  case GAME_PLAY:
    getEvents();
    l = doMovement(0, game2->time.dt); /* this won't generate new events */
    if(l != NULL) {
      fprintf(stderr, "something is seriously wrong - ignoring events\n");
    }
    break;
  }
  /* fprintf(stderr, "game time: %.3f\n", game2->time.current / 1000.0); */
}

/* create an event and put it into the global event queue */

void createTurnEvent(int player, int direction) {
  GameEvent *e;
  list *p;

  for(p = &(game2->events); p->next != NULL; p = p->next);
  e = (GameEvent*) malloc(sizeof(GameEvent));
  p->data = e;
  p->next = (list*) malloc(sizeof(list));
  p->next->next = NULL;
  switch(direction) {
  case TURN_LEFT: e->type = EVENT_TURN_LEFT; break;
  case TURN_RIGHT: e->type = EVENT_TURN_RIGHT; break;
  }
  e->x = game->player[player].data->iposx;
  e->y = game->player[player].data->iposy;
  e->player = player;
  e->timestamp = game2->time.current;
}
