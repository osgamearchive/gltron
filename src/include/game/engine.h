#ifndef ENGINE_H
#define ENGINE_H

typedef struct GameEvent GameEvent;

void doLeftTurn(GameEvent *e);
void doRightTurn(GameEvent *e);
void doCrashPlayer(GameEvent *e);
void game_ResetData(void);

#endif
