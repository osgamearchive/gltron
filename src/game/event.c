#include "video/video.h"
#include "game/game.h"
#include "audio/audio.h"
#include "Nebu_scripting.h"

/*! \fn int processEvent(GameEvent* e)
  handle events, e.g.
  left/right/crash/stop

  frees e
*/

int processEvent(GameEvent* e) {
  int value = 0;
  Data *data;

#ifdef RECORD
  if(game2->mode == GAME_SINGLE_RECORD) {
    writeEvent(e);
  }
#endif
  switch(e->type) {
  case EVENT_TURN_LEFT:
    doTurn(e, TURN_LEFT);
    break;
  case EVENT_TURN_RIGHT:
    doTurn(e, TURN_RIGHT);
    break;
  case EVENT_CRASH: 
    data = game->player[e->player].data;
    data->posx = e->x;
    data->posy = e->y;
    displayMessage(TO_CONSOLE, "player %d crashed", e->player + 1);
    doCrashPlayer(e);
    break;
  case EVENT_STOP:
    // displayMessage(TO_STDOUT, "game stopped");
#ifdef RECORD
    if(game2->mode == GAME_SINGLE_RECORD) {
      stopRecording();
      game2->mode = GAME_SINGLE;
    } else if(game2->mode == GAME_PLAY) {
      stopPlaying();
      game2->mode = GAME_SINGLE;
    }
#endif
    if(e->player<PLAYERS && game->player[e->player].ai->active != AI_NONE) {
      game->winner = e->player;
      displayMessage(TO_CONSOLE, "winner: %d", game->winner + 1);
    } else {
      game->winner = -2;
      displayMessage(TO_CONSOLE, "everyone died! no one wins!");
    }
		SystemExitLoop(RETURN_GAME_END);
    game->pauseflag = PAUSE_GAME_FINISHED;
    value = 1;
    break;
  }
  free(e);
  return value;
}

/*! \fn static list* doMovement(int mode, int dt)
  do physics, create CRASH and STOP events
*/

List* doMovement(int mode, int dt) {
  int i;
  List *l = NULL;

  for(i = 0; i < game->players; i++) {
		Data *data = game->player[i].data;
		PlayerVisual *pV = gPlayerVisuals + i;
    if(data->speed > 0) { /* still alive */
			float fs;
			float t;
      fs = 1.0 - SPEED_OZ_FACTOR + SPEED_OZ_FACTOR * 
				cos(i * M_PI / 4.0 + 
						(float)(game2->time.current % SPEED_OZ_FREQ) * 
						2.0 * M_PI / (float)SPEED_OZ_FREQ);

      t = dt / 100.0 * data->speed * fs;
      while(t > 0) {
				if(t < 1) {
					float posx = floorf(data->posx);
					float posy = floorf(data->posy);
					data->posx += t * dirsX[data->dir];
					data->posy += t * dirsY[data->dir];
					t = 0;
					// don't write to the same pixel again
					if(posx == floorf(data->posx) &&
						 posy == floorf(data->posy))
						 break;
				}	else {
					 data->posx += 1.0f * dirsX[data->dir];
					 data->posy += 1.0f * dirsY[data->dir];
					 t -= 1.0f;
				}
				if(getCol( floorf(data->posx), floorf(data->posy)) && mode) {
					createEvent(i, EVENT_CRASH);
					break;
				} else {
					writePosition(i);
				}
      }
    } else { /* already crashed */
      if(game2->rules.eraseCrashed == 1 && data->trail_height > 0)
				data->trail_height -= (float)(dt * TRAIL_HEIGHT) / 1000;
      if(pV->exp_radius < EXP_RADIUS_MAX)
				pV->exp_radius += (float)dt * EXP_RADIUS_DELTA;
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
            createEvent(winner, EVENT_STOP);
						/* a stop event is the last event that happens */
						return l;
					}
				}
      }
    }      
  }
  return l;
}
 
/*! \fn void idleGame( void )
  game loop:
  run ai, process events, do physics, process events again,
  do camera movement
*/

void Game_Idle(void) {
  List *l;
  List *p;
  int i;
  int dt;
  int t;

	switch(game2->mode) {
	case GAME_SINGLE:
#ifdef RECORD
	case GAME_SINGLE_RECORD:
#endif
		/* check for fast finish */
    
		if (game2->settingsCache.fast_finish == 1) {
			int factors[4] = { 4, 6, 12, 25 };
			int threshold[4] = { 0, 300, 600, 800 };
			int factor = 1;
			for(i = 0; i < 4; i++) {
				if(game2->rules.grid_size > threshold[i])
					factor = factors[i];
			}
			for (i = 0; i < game->players; i++) {
				if (game->player[i].ai->active != AI_COMPUTER &&
						gPlayerVisuals[i].exp_radius < EXP_RADIUS_MAX) {
					factor = 1;
				}
			}
			dt = game2->time.dt * factor;
		} else { 
			dt = game2->time.dt;
		}

		while(dt > 0) {
			if(dt > PHYSICS_RATE) t = PHYSICS_RATE;
			else t = dt;

			/* run AI */
			for(i = 0; i < game->players; i++)
				if(game->player[i].ai != NULL)
					if(game->player[i].ai->active == AI_COMPUTER &&
						 PLAYER_IS_ACTIVE(&game->player[i])) {

						if (game2->settingsCache.ai_level < 2) {
							doComputer(i, 0);
						} else {
							doComputer2(i, 0);
						} 
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
#ifdef RECORD
	case GAME_PLAY_NETWORK:
		/* fall through to GAME_PLAY */
	case GAME_PLAY:
		getEvents(); 
		l = doMovement(0, game2->time.dt); /* this won't generate new events */
		if(l != NULL) {
			fprintf(stderr, "something is seriously wrong - ignoring events\n");
		}
		break;
#endif /* RECORD */
	}
    
	doCameraMovement();
	doRecognizerMovement();
}

/*! \fn void createEvent(int player, event_type_e eventType)
  helper function to create an event struct and insert it into the
  global event queue
*/

void createEvent(int player, event_type_e eventType) {
  GameEvent *e;
  List *p = &(game2->events);

  /* move to the end of the event list */
  while (p->next)
    p = p->next;

  /* TODO: handle failed malloc */
  e = (GameEvent*) malloc(sizeof(GameEvent));
  p->data = e;
  p->next = (List*) malloc(sizeof(List));
  p->next->next = NULL;
  e->type = eventType;
  e->x = game->player[player].data->posx;
  e->y = game->player[player].data->posy;
  e->player = player;
  e->timestamp = game2->time.current;
}
