#ifndef ENGINE_H
#define ENGINE_H

#include "event.h"

extern void doLeftTurn(GameEvent *e);
extern void doRightTurn(GameEvent *e);
extern void doCrashPlayer(GameEvent *e);
extern void game_ResetData(void);

#endif
