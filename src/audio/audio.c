#include "audio/audio.h"
#include "game/game.h"

void resetAudioData() {
	int i;
	for(i = 0; i < game->players; i++)
		if( game->player[i].data->speed > 0)
      Audio_StartEngine(i);
}

