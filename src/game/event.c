#include <math.h>
#include <float.h>

#include "video/video.h"
#include "game/game.h"
#include "audio/audio.h"
#include "Nebu_scripting.h"
#include "Nebu_base.h"

/*! \fn int processEvent(GameEvent* e)
  handle events, e.g.
  left/right/crash/stop

  frees e
*/

int processEvent(GameEvent* e) {
	int value = 0;

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
		nebu_System_ExitLoop(RETURN_GAME_END);
		game->pauseflag = PAUSE_GAME_FINISHED;
		value = 1;
		break;
	}
	free(e);
	return value;
}

int crashTestPlayers(int i, const segment2 *movement) {
	int j, k;
	int crash = 0;
	Data *data = game->player[i].data;
	segment2 *current = data->trails + data->trailOffset;
	// debug: only test player0 against himself
	// j = 0; 
	// if(i == 0) { 
	for(j = 0; j < game->players; j++) {
		int crash = 0;

		if(game->player[j].data->trail_height < TRAIL_HEIGHT)
			continue;

		for(k = 0; k < game->player[j].data->trailOffset + 1; k++) {
			segment2 *wall;
			vec2 v;
			float t1, t2;
						
			if(j == i && k >= game->player[j].data->trailOffset - 1)
				break;

			wall = game->player[j].data->trails + k;
						
			if(segment2_Intersect(&v, &t1, &t2, movement, wall)) {
#if 0
				printf("(%.2f, %.2f), (%.2f, %.2f), %.2f, %.2f\n",
							 data->posx, data->posy,
							 v.v[0], v.v[1],
							 t1, t2); 
#endif
				if(t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
					current->vDirection.v[0] = v.v[0] - current->vStart.v[0];
					current->vDirection.v[1] = v.v[1] - current->vStart.v[1];
					createEvent(i, EVENT_CRASH);
					crash = 1;
					break;
				}
			}
		}
		if(crash)
			break;
	}
	return crash;
}

int crashTestWalls(int i, const segment2 *movement) {
	int j;
	vec2 v;
	float t1, t2;
	int crash = 0;

	Data *data = game->player[i].data;
	segment2 *current = data->trails + data->trailOffset;
	
	for(j = 0; j < game2->level->nBoundaries; j++) {
		if(segment2_Intersect(&v, &t1, &t2, current, game2->level->boundaries + j)) {
			if(t1 >= 0 && t1 < 1 && t2 >= 0 && t2 < 1) {
				current->vDirection.v[0] = v.v[0] - current->vStart.v[0];
				current->vDirection.v[1] = v.v[1] - current->vStart.v[1];
				createEvent(i, EVENT_CRASH);
				crash = 1;
				break;
			}
		}
	}
	return crash;
}

int applyBooster(int player, int dt) {
	Data *data = game->player[player].data;
	if(data->booster > 0 && data->boost_enabled) {
		float boost = getSettingf("booster_use") * dt / 1000.0f;
		if(boost > data->booster) {
			boost = data->booster;
			data->boost_enabled = 0;
		}
		data->speed += boost;
		data->booster -= boost;
		return 1;
	}
	else {
		float booster_max = getSettingf("booster_max");
		if(data->booster < booster_max) {
			data->booster += getSettingf("booster_regenerate") * dt / 1000.0f;
			if(data->booster > booster_max)
				data->booster = booster_max;
		}
		return 0;
	}
}

int applyWallBuster(int player, int dt) {
	Data *data = game->player[player].data;
	if(data->wall_buster_enabled) {
		// printf("%i: wall buster enabled\n", player);
	}
	if(data->wall_buster > 0 && data->wall_buster_enabled) {
		// printf("using buster: %.2f\n", data->wall_buster);
		float wall_buster = getSettingf("wall_buster_use") * dt / 1000.0f;
		if(wall_buster > data->wall_buster) {
			wall_buster = data->wall_buster;
			data->wall_buster_enabled = 0;
		}
		data->wall_buster -= wall_buster;
		// printf("buster: %.2f\n\n", data->wall_buster);
		return 1;
	}
	else {
		float wall_buster_max = getSettingf("wall_buster_max");
		if(data->wall_buster < wall_buster_max) {
			data->wall_buster += getSettingf("wall_buster_regenerate") * dt / 1000.0f;
			if(data->wall_buster > wall_buster_max)
				data->wall_buster = wall_buster_max;
		}
		// printf("regenerating buster: %.2f\n", data->wall_buster);
		return 0;
	}
}

void applyDecceleration(int player, int dt, float factor) {
	Data *data = game->player[player].data;
	if(data->speed > game2->rules.speed) {
		float decrease = factor * dt / 1000.0f;
		data->speed -= decrease;
		if(data->speed < game2->rules.speed)
			data->speed = game2->rules.speed;
	}
}

int applyWallAcceleration(int player, int dt) {
	// find distance to enemy walls left & right
	enum { eLeft, eRight, eMax };
	segment2 segments[eMax];

	Data *data = game->player[player].data;
	int dirLeft = (data->dir + 3) % 4;
	int dirRight = (data->dir + 1) % 4;

	float left, right;

	float x, y;
	vec2 vPos;

	int i, j;

	getPositionFromIndex(&x, &y, player);
	vPos.v[0] = x;
	vPos.v[1] = y;

	for(i = 0; i < eMax; i++) {
		vec2_Copy(&segments[i].vStart, &vPos);
	}

	segments[eLeft].vDirection.v[0] = (float) dirsX[dirLeft];
	segments[eLeft].vDirection.v[1] = (float) dirsY[dirLeft];
	segments[eRight].vDirection.v[0] = (float) dirsX[dirRight];
	segments[eRight].vDirection.v[1] = (float) dirsY[dirRight];

	left = FLT_MAX;
	right = FLT_MAX;

	for(i = 0; i < game->players; i++) {
		segment2 *wall = game->player[i].data->trails;

		if(i == player)
			continue;
		if(game->player[i].data->trail_height < TRAIL_HEIGHT)
			continue;
		
		for(j = 0; j < game->player[i].data->trailOffset + 1; j++) {
			float t1, t2;
			vec2 v;
			if(segment2_Intersect(&v, &t1, &t2, segments + eLeft, wall) &&
				 t1 > 0 && t1 < left && t2 >= 0 && t2 <= 1)
				left = t1;
			if(segment2_Intersect(&v, &t1, &t2, segments + eRight, wall) &&
				 t1 > 0 && t1 < right && t2 >= 0 && t2 <= 1)
				right = t1;
			wall++;
		}
	}

	{
		float accell_max = getSettingf("wall_accel_max");
		if(left < accell_max || right < accell_max) {
			float boost = getSettingf("wall_accel_use") * dt / 1000.0f;
			data->speed += boost;
			return 1;
		} else {
			return 0;
		}
	}
}

/*! \fn void doMovement(int dt)
	\param dt	time for this tick

	do physics, create CRASH and STOP events
*/

void doMovement(int dt)
{
	int i;

	for(i = 0; i < game->players; i++) { // foreach player
		Data *data = game->player[i].data;
		PlayerVisual *pV = gPlayerVisuals + i;
		if(data->speed > 0) { // still alive
			float fs;
			float t;

			float deccel = 0;
			if(getSettingf("wall_accel_on") == 1) { // wall acceleration
				if(!applyWallAcceleration(i, dt)) {
					deccel = getSettingf("wall_accel_decrease");
				}
				else {
					deccel = -1; // forbid deacceleration for booster
				}
			} // wall acceleration

			if(getSettingf("wall_buster_on") == 1) { // wall buster
				// printf("applying wallbuster for player %i\n");
				applyWallBuster(i, dt);
			} // wall buster

			if(getSettingf("booster_on") == 1) { // booster
				if(!applyBooster(i, dt) && deccel != -1) {
					float d = getSettingf("booster_decrease");
					deccel = d > deccel ? d : deccel;
				} else {
					deccel = -1;
				}
			} // booster

			if(deccel > 0)
				applyDecceleration(i, dt, deccel);

			// if(i == 0)
			// printf("speed: %.2f, boost: %.2f\n", data->speed, data->booster);

			fs = 1.0f - SPEED_OZ_FACTOR + SPEED_OZ_FACTOR * 
				cosf(i * PI / 4.0f + 
				(game2->time.current % SPEED_OZ_FREQ) * 
				2.0f * PI / SPEED_OZ_FREQ);

			t = dt / 100.0f * data->speed * fs;

			{	// movement
				segment2 *current = data->trails + data->trailOffset;
				segment2 movement;
				int crash = 0;
				float x, y;

				getPositionFromData(&x, &y, data);
				movement.vStart.v[0] = x;
				movement.vStart.v[1] = y;
				movement.vDirection.v[0] = t * dirsX[data->dir];
				movement.vDirection.v[1] = t * dirsY[data->dir];

				current->vDirection.v[0] += t * dirsX[data->dir];
				current->vDirection.v[1] += t * dirsY[data->dir];

				if(!data->wall_buster_enabled) { // collision detection against players
					crash = crashTestPlayers(i, &movement);
					if(crash) {
						printf("player %d crashed into other players \n", i);
						printf("%f %f %f %f\n",
							movement.vStart.v[0],
							movement.vStart.v[1],
							movement.vDirection.v[0],
							movement.vDirection.v[1]);
					} // crash debug output
				} // collision detection against players
				if(!crash) { // collision detection against walls
					crash = crashTestWalls(i, &movement);
					if(crash) {
						printf("player %d crashed into the walls\n", i);
						printf("%f %f %f %f\n",
							movement.vStart.v[0],
							movement.vStart.v[1],
							movement.vDirection.v[0],
							movement.vDirection.v[1]);
					} // crash debut output
				} // collision detection against walls
			} // movement
		} // still alive
		else { // already crashed
			if(game2->rules.eraseCrashed == 1 && data->trail_height > 0)
				data->trail_height -= (dt * TRAIL_HEIGHT) / 1000.0f;
			if(pV->exp_radius < EXP_RADIUS_MAX)
				pV->exp_radius += dt * EXP_RADIUS_DELTA;
			else if (data->speed == SPEED_CRASHED) { // done exploding
				int winner = -1;

				data->speed = SPEED_GONE;
				game->running--;
				if(game->running <= 1) { // all dead
					/* find survivor, create winner event */
					int i;
					float maxSpeed = SPEED_GONE;
					for(i = 0; i < game->players; i++) {
						if(game->player[i].data->speed >= maxSpeed) {
							winner = i;
							maxSpeed = game->player[i].data->speed;
						}
					}
					createEvent(winner, EVENT_STOP);
					/* a stop event is the last event that happens */
					return;
				} // all dead
			} // done exploding
		} // already crashed
	} // foreach player
}
 
/*! \fn void idleGame( void )
  game loop:
  run ai, process events, do physics, process events again,
  do camera movement
*/

void Game_Idle(void) {
	nebu_List *p;
	int i;
	int dt; // time since last frame
	int t; // time for a single tick to be processed

	/* check for fast finish */
	if (gSettingsCache.fast_finish != 1)
	{
		// time since last idle processing
		dt = game2->time.dt;
	}
	else
	{
		// compute an acceleration factor (speedup time by factor 4/6/12/25)
		// depending on the size of the arena
		int factors[4] = { 4, 6, 12, 25 };
		int threshold[4] = { 0, 300, 600, 800 };
		int factor = 1;
		for(i = 0; i < 4; i++)
		{
			if(box2_Diameter(&game2->level->boundingBox) > threshold[i])
				factor = factors[i];
		}
		for (i = 0; i < game->players; i++)
		{
			if (game->player[i].ai->active != AI_COMPUTER &&
				gPlayerVisuals[i].exp_radius < EXP_RADIUS_MAX)
			{
				factor = 1;
			}
		}
		dt = game2->time.dt * factor;
	} 

	while(dt > 0) {
		// chop time since last frame into ticks of maximally PHYSICS_RATE milliseconds
		if(dt > PHYSICS_RATE) t = PHYSICS_RATE;
		else t = dt;

		/* run AI */
		for(i = 0; i < game->players; i++)
		{
			if(game->player[i].ai != NULL)
			{
				if(game->player[i].ai->active == AI_COMPUTER &&
					PLAYER_IS_ACTIVE(&game->player[i]))
				{
					doComputer(i, 0);
					// schedules events (e.g. turns)
				}
			}
		}

		/* process any outstanding events (turns, etc) */
		for(p = &(game2->events); p->next != NULL; p = p->next) {
			if(processEvent((GameEvent*) p->data))
				// a STOP event was encountered, exiting
				// TODO: fix, because this leaks the list element, and also all unprocessed turn events!
				return;
		}

		/* free list items (processed events are already freed in processEvents */
		p = game2->events.next;
		while(p != NULL) {
			nebu_List *l = p;
			p = p->next;
			free(l);
		}
		game2->events.next = NULL;

		doMovement(t); /* this can generate new events */

		dt -= t;
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
	nebu_List *p = &(game2->events);

	/* move to the end of the event list */
	while (p->next)
		p = p->next;

	// add event to list
	e = (GameEvent*) malloc(sizeof(GameEvent));
	p->data = e;
	p->next = (nebu_List*) malloc(sizeof(nebu_List));
	p->next->next = NULL;

	// store event information
	e->type = eventType;
	getPositionFromIndex(&e->x, &e->y, player);
	e->player = player;
	e->timestamp = game2->time.current;
}
