#include "game/game.h"
#include "video/video.h"
#include "audio/audio.h"
#include "Nebu_base.h"

#include <math.h>

int getCol(int x, int y) {
  return x < 1 || x >= game2->rules.grid_size ||
		y < 1 || y >= game2->rules.grid_size ||
		colmap[ y * colwidth + x ];
}

void initGameStructures(void) { /* called only once */
  int i;

  game->winner = -1;

  game->players = PLAYERS;
  game->player = (Player *) malloc(MAX_PLAYERS * sizeof(Player));

  for(i = 0; i < game->players; i++) {
		Player *p = game->player + i;

    p->ai = (AI*) malloc(sizeof(AI));
    p->data = (Data*) malloc(sizeof(Data));
    p->data->trails = (Line*) malloc(MAX_TRAIL * sizeof(Line));
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
		ai->moves = getSettingi("grid_size") / 10;
		ai->danger = 0;
		ai->lastx = 0;
		ai->lasty = 0;

		/* arrange players in circle around center */

		/* randomize position on the grid */
		data->posx = startpos[ startIndex[i] ][0] * getSettingi("grid_size");
		data->posy = startpos[ startIndex[i] ][1] * getSettingi("grid_size");
		if(i == 0) data->posy -= 1;
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

		data->trails[ data->trailOffset ].sx = floorf(data->posx);
		data->trails[ data->trailOffset ].sy = floorf(data->posy);
		
		data->trails[ data->trailOffset ].ex = floorf(data->posx);
		data->trails[ data->trailOffset ].ey = floorf(data->posy);

		{
			int camType;
			Camera *cam = game->player[i].camera;
			camType = (game->player[i].ai->active == AI_COMPUTER) ? 
				CAM_CIRCLE : game2->settingsCache.camType;
			initCamera(cam, data, camType);
		}
	}

	free(startIndex);

	game->running = game->players - not_playing; /* not everyone is alive */
	/* printf("starting game with %d players\n", game->running); */
	game->winner = -1;
}

void initData(void) {
	int i;
  
	/* colmap */

	/* TODO: check if grid_size/colwidth has changed and  
	 *       reallocate colmap accordingly                */

	colwidth = getSettingi("grid_size");
	if(colmap != NULL)
		free(colmap);
  
	colmap = (unsigned char*) malloc(colwidth*colwidth/*getSettingi("grid_size")*/);
	for(i = 0; i < colwidth * colwidth/*getSettingi("grid_size")*/; i++)
		*(colmap + i) = 0;
  
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

void clearTrail(int player) {
	int i;

	for(i = 0; i < colwidth *  game2->rules.grid_size; i++)
		if(colmap[i] == player + 1)
			colmap[i] = 0;
}

void doCrashPlayer(GameEvent *e) {
	int j;

	Audio_CrashPlayer(e->player);
	Audio_StopEngine(e->player);

	for(j = 0; j < game->players; j++) 
		if(j != e->player && PLAYER_IS_ACTIVE(&(game->player[j])))
			game->player[j].data->score++;

	game->player[e->player].data->speed = SPEED_CRASHED;

	if(game2->rules.eraseCrashed == 1)
		clearTrail(e->player);
}

void writePosition(int player) {
	int x, y;

	x = floorf(game->player[player].data->posx);
	y = floorf(game->player[player].data->posy);

	/* collision detection */
	colmap[ y * colwidth + x ] = player + 1;
}

void newTrail(Data* data) {
	data->trails[data->trailOffset].ex = data->posx;
	data->trails[data->trailOffset].ey = data->posy;

	data->trailOffset++;
	
	data->trails[data->trailOffset].sx = data->posx;
	data->trails[data->trailOffset].sy = data->posy;

	data->trails[data->trailOffset].ex = data->posx;
	data->trails[data->trailOffset].ey = data->posy;
}
      
void doTurn(GameEvent *e, int direction) {
	Data *data = game->player[e->player].data;
	newTrail(data);
	data->last_dir = data->dir;
	data->dir = (data->dir + direction) % 4;
	data->turn_time = game2->time.current;
}
