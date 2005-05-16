#ifndef ENGINE_H
#define ENGINE_H

#include "game/event.h" // GameEvent
#include "game/game_data.h" // Game, Game2

void doLeftTurn(GameEvent *e);
void doRightTurn(GameEvent *e);
void doCrashPlayer(GameEvent *e);
void game_ResetData(void);
void game_FreePlayers(Game *pGame, Game2 *pGame2);
void game_CreatePlayers(int players, Game **ppGame, Game2 **ppGame2);

#endif
