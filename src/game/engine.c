#include "game/game.h"
#include "video/video.h"
#include "audio/audio.h"
#include "Nebu_base.h"

#include <math.h>

void getPositionFromIndex(float *x, float *y, int player) {
	getPositionFromData(x, y, game->player[player].data);
}

void getPositionFromData(float *x, float *y, Data *data) {
	vec2 v;
	vec2Add(&v,
					&data->trails[data->trailOffset].vStart,
					&data->trails[data->trailOffset].vDirection);
	*x = v.v[0];
	*y = v.v[1];
}

void initGameStructures(void) { /* called only once */
  int i;

  /* initialize some global variables */
  game2 = &main_game2;
  game = &main_game;
  game->pauseflag = PAUSE_NO_GAME;

  game->winner = -1;

  game->players = PLAYERS;
  game->player = (Player *) malloc(MAX_PLAYERS * sizeof(Player));

  for(i = 0; i < game->players; i++) {
		Player *p = game->player + i;

    p->ai = (AI*) malloc(sizeof(AI));
    p->data = (Data*) malloc(sizeof(Data));
    p->data->trails = (segment2*) malloc(MAX_TRAIL * sizeof(segment2));
		p->data->trailOffset = 0;
		p->camera = (Camera*) malloc(sizeof(Camera));
   }

  game2->events.next = NULL;
  game2->mode = GAME_SINGLE;
}

void resetPlayerData(void) {
  int i;
  Data *data;
  AI *ai;
  int not_playing = 0;

  int *startIndex;
  startIndex = malloc( game->players * sizeof(int) );
  randomPermutation(game->players, startIndex);

  for(i = 0; i < game->players; i++) {
    float startpos[][2] = { 
			{ 0.5, 0.25 }, { 0.75, 0.5 }, { 0.5, 0.75 }, { 0.25, 0.5 }
    };
		float x, y;

    data = game->player[i].data;
    ai = game->player[i].ai;
    /* init ai */

		switch(i) {
		case 0: ai->active = getSettingi("ai_player1"); break;
		case 1: ai->active = getSettingi("ai_player2"); break;
		case 2: ai->active = getSettingi("ai_player3"); break;
		case 3: ai->active = getSettingi("ai_player4"); break;
		default:
			fprintf(stderr, "[error] player index #%d not caught!\n", i);
			ai->active = AI_NONE;
		}
		ai->tdiff = 0;

		/* arrange players in circle around center */

		/* randomize position on the grid */
		x = startpos[ startIndex[i] ][0] * getSettingi("grid_size");
		y = startpos[ startIndex[i] ][1] * getSettingi("grid_size");
		/* randomize starting direction */
		data->dir = trand() & 3;
		/* data->dir = startdir[i]; */
		data->last_dir = data->dir;

		/* if player is playing... */
		if(ai->active != AI_NONE) {
			data->speed = getSettingf("speed");
			data->booster = getSettingf("booster_max");
			data->boost_enabled = 0;
			data->trail_height = TRAIL_HEIGHT;
		} else {
			data->speed = SPEED_GONE;
			data->trail_height = 0;
			not_playing++;
		}
		// data->trail = data->trails;
		data->trailOffset = 0;

		data->trails[ data->trailOffset ].vStart.v[0] = x;
		data->trails[ data->trailOffset ].vStart.v[1] = y;
		
		data->trails[ data->trailOffset ].vDirection.v[0] = 0;
		data->trails[ data->trailOffset ].vDirection.v[1] = 0;

		{
			int camType;
			Camera *cam = game->player[i].camera;
			camType = (game->player[i].ai->active == AI_COMPUTER) ? 
				CAM_CIRCLE : gSettingsCache.camType;
			initCamera(cam, data, camType);
		}
	}

	free(startIndex);

	game->running = game->players - not_playing; /* not everyone is alive */
	/* printf("starting game with %d players\n", game->running); */
	game->winner = -1;
}

void initData(void) {
	/* lasttime = SystemGetElapsedTime(); */
	game->pauseflag = PAUSE_GAME_RUNNING;

	game2->rules.speed = getSettingf("speed");
	game2->rules.eraseCrashed = getSettingi("erase_crashed");
	game2->rules.grid_size = getSettingi("grid_size");

	game2->grid.width = game2->rules.grid_size;
	game2->grid.height = game2->rules.grid_size;

	/* time management */
	game2->time.lastFrame = 0;
	game2->time.current = 0;
	game2->time.offset = SystemGetElapsedTime();
	/* TODO: fix that */
	game2->players = game->players;
	/* event management */
	game2->events.next = NULL;
	/* TODO: free any old events that might have gotten left */

  resetVideoData();
	resetPlayerData();

  initWalls();
}

void Time_Idle(void) {
	game2->time.lastFrame = game2->time.current;
	game2->time.current = SystemGetElapsedTime() - game2->time.offset;
	game2->time.dt = game2->time.current - game2->time.lastFrame;
	/* fprintf(stderr, "dt: %d\n", game2->time.dt); */
}

void resetScores(void) {
	int i;
	for(i = 0; i < game->players; i++)
		game->player[i].data->score = 0;
}

void doCrashPlayer(GameEvent *e) {
	int j;

	Audio_CrashPlayer(e->player);
	Audio_StopEngine(e->player);

	for(j = 0; j < game->players; j++) 
		if(j != e->player && PLAYER_IS_ACTIVE(&(game->player[j])))
			game->player[j].data->score++;

	game->player[e->player].data->speed = SPEED_CRASHED;
}

void newTrail(Data* data) {
	segment2 *s;
	float x, y;

	getPositionFromData(&x, &y, data);

	data->trailOffset++;
	s = data->trails + data->trailOffset;

	s->vStart.v[0] = x;
	s->vStart.v[1] = y;
	s->vDirection.v[0] = 0;
	s->vDirection.v[1] = 0;
	
}
      
void doTurn(GameEvent *e, int direction) {
	Data *data = game->player[e->player].data;
	newTrail(data);
	data->last_dir = data->dir;
	data->dir = (data->dir + direction) % 4;
	data->turn_time = game2->time.current;
}

void initWalls(void) {
	float raw[4][4] = {
		{ 0.0f, 0.0f, 1.0f,  0.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, -1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, -1.0f }
	};

	float width = game2->rules.grid_size;
	float height = game2->rules.grid_size;
	
	int j;

	for(j = 0; j < 4; j++) {
		walls[j].vStart.v[0] = raw[j][0] * width;
		walls[j].vStart.v[1] = raw[j][1] * height;
		walls[j].vDirection.v[0] = raw[j][2] * width;
		walls[j].vDirection.v[1] = raw[j][3] * height;
	}
}
