#ifndef GAME_H
#define GAME_H

typedef struct Game Game;
typedef struct Game2 Game2;
typedef struct GameEvent GameEvent;
typedef struct Data Data;

#define NEW_LEVEL

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
	RETURN_QUIT = 0,
	RETURN_GAME_END,
	RETURN_GAME_PAUSE,
	RETURN_GAME_UNPAUSE,
	RETURN_GAME_CREDITS,
	RETURN_GAME_ESCAPE,
	RETURN_GUI_PROMPT,
	RETURN_TIMEDEMO,
	RETURN_TIMEDEMO_ABORT,
	RETURN_CREDITS,
	RETURN_GAME_LAUNCH,
	RETURN_GUI_ESCAPE,
	RETURN_PROMPT_ESCAPE,
	RETURN_PAUSE_ESCAPE
};

/* physics recomputed every 10ms */
#define PHYSICS_RATE 20


#define PLAYERS 4
#define MAX_PLAYERS 4
#define MAX_TRAIL 1000


/* these must be < 0 */
#define SPEED_CRASHED -1
#define SPEED_GONE -2

#define TRAIL_HEIGHT 3.5f

/* game options */
#define SPEED_OZ_FREQ 1200
#define SPEED_OZ_FACTOR 0.09f

#define TURN_LENGTH 200

#define FAST_FINISH 40

enum {
	PAUSE_NO_GAME = 0,
	PAUSE_GAME_RUNNING = 1,
	PAUSE_GAME_FINISHED = 2,
	PAUSE_GAME_SUSPENDED = 4,
	PAUSE_GAME_STARTING = 8,
	PAUSE_DEMO_NEW = 16,
	PAUSE_DEMO_RUNNING = 32,
};

extern Game main_game;
extern Game *game;
extern Game2 main_game2;
extern Game2 *game2;

extern int dirsX[];
extern int dirsY[];

extern float default_speeds[];
extern int default_arena_sizes[];

/* ai -> computer.c */

void loadLevel(void);

void initGameAI(int level);
void doComputer(int player, int target);

void resetScores(void);

void doTurn(GameEvent* e, int direction);

void doMovement(int dt);
void writePosition(int player);

void initGameStructures(void);
void initGameLevel(void);
void initClientData(void);

void Game_Idle(void);
void enterGame(void);
void exitGame(void);

void gameMouse(int buttons, int state, int x, int y);
void video_ResetData(void);
void resetPlayerData(void);

void initWalls(void);

void getPositionFromIndex(float *x, float *y, int player);
void getPositionFromData(float *x, float *y, Data *data);
#endif
