#ifndef DATA_H
#define DATA_H

#include "basic_types.h"
#include "game_data.h"
#include "client_data.h"

/* general data structures */
  
#define PLAYER_IS_ACTIVE(x) ((x)->data->speed > 0)

enum {
  GAME_SINGLE = 1,
#ifdef RECORD
  GAME_SINGLE_RECORD = 2,
  GAME_PLAY = 4,
  GAME_PLAY_NETWORK = 8,
  GAME_NETWORK_RECORD
#endif
};

#endif
