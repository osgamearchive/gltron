#ifndef GAME_H
#define GAME_H

#include "game_data.h"
#include "engine.h"
#include "event.h"

#include "Nebu_base.h"

/* use enums, not magic */
enum {
	TURN_LEFT = 3,
	TURN_RIGHT = 1
};

enum { 
	AI_HUMAN = 0,
	AI_COMPUTER = 1,
	AI_NONE = 2
};

enum {
	RETURN_GAME_LAUNCH = 0,
	RETURN_GAME_END,
	RETURN_GAME_PAUSE,
	RETURN_GAME_UNPAUSE,
	RETURN_GAME_CREDITS,
	RETURN_GAME_ESCAPE,
	RETURN_GUI_PROMPT,
	RETURN_TIMEDEMO,
	RETURN_TIMEDEMO_ABORT,
	RETURN_CREDITS,
	RETURN_QUIT,
	RETURN_GUI_ESCAPE,
	RETURN_PROMPT_ESCAPE,
	RETURN_PAUSE_ESCAPE,
};

/* physics recomputed every 10ms */
#define PHYSICS_RATE 20


#define PLAYERS 4
#define MAX_PLAYERS 4
#define MAX_TRAIL 1000


/* these must be < 0 */
#define SPEED_CRASHED -1
#define SPEED_GONE -2

#define TRAIL_HEIGHT 3.5

/* game options */
#define SPEED_OZ_FREQ 1200
#define SPEED_OZ_FACTOR 0.09

#define TURN_LENGTH 200

#define FAST_FINISH 40

#define PAUSE_GAME_FINISHED 1

extern Game main_game;
extern Game *game;
extern Game2 main_game2;
extern Game2 *game2;

extern unsigned char* colmap;
extern int colwidth;

extern int dirsX[];
extern int dirsY[];

extern float default_speeds[];
extern int default_arena_sizes[];

/* ai -> computer.c */

extern void initGameAI(int level);
extern void doComputer(int player, int target);
extern void doComputer2(int player, int target);

extern void resetScores();

extern void doTurn(GameEvent* e, int direction);

extern List* doMovement(int mode, int dt);
extern void writePosition(int player);

extern void initGameStructures();
extern void initClientData();

extern void Game_Idle();
extern void enterGame();
extern void exitGame();

extern void gameMouseMotion(int x, int y);
extern void gameMouse(int buttons, int state, int x, int y);
extern void resetVideoData();
extern void resetPlayerData();
#endif
