#include "server_gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

void initDefaultSettings() {
  game->pauseflag = 0;

  game->settings->erase_crashed = 1;
  game->settings->fast_finish = 1;
  game->settings->current_speed = 7.0;
  game->settings->game_speed = 2; /* fast */
  /* game->settings->grid_size MUST be divisible by 8 */
  game->settings->arena_size = 3;
  game->settings->grid_size = 600;

  game->settings->ai_player1 = 0;
  game->settings->ai_player2 = 1;
  game->settings->ai_player3 = 1;
  game->settings->ai_player4 = 1;

  game->settings->ai_level = 1;
  
  game->settings->grid_size = default_arena_sizes[game->settings->arena_size];

  /* choose speed */
  default_speeds[4] = game->settings->current_speed;
  game->settings->current_speed = default_speeds[ game->settings->game_speed ];
}

void initMainGameSettings() {
  game2 = &main_game2;
  game = &main_game;
  game->settings = (Settings*) malloc(sizeof(Settings));

  game2->network.status = 0;

  /* initialize defaults, then load modifications from file */
  initDefaultSettings();

  /* sanity check: speed, grid_size */
  if(game->settings->current_speed <= 0) {
    fprintf(stderr, "[gltron] sanity check failed: speed = %.2ff\n",
	    game->settings->current_speed);
    game->settings->current_speed = 6.0;
    fprintf(stderr, "[gltron] reset speed: speed = %.2f\n",
	    game->settings->current_speed);
  }
  if(game->settings->grid_size % 8) {
    fprintf(stderr, "[gltron] sanity check failed: grid_size %% 8 != 0: "
	    "grid_size = %d\n", game->settings->grid_size);
    game->settings->grid_size = 240;
    fprintf(stderr, "[gltron] reset grid_size: grid_size = %d\n",
	    game->settings->grid_size);
  }
}




