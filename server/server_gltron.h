/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef GLTRON_H
#define GLTRON_H
#include "server_data.h"
#include "server_system.h"

/* use enums, not magic */

enum { 
  AI_HUMAN = 0,
  AI_COMPUTER = 1,
  AI_NONE = 2
  
};

/* global constants */

/* physics recomputed every 10ms */
#define PHYSICS_RATE 20

#define PLAYERS 4
#define MAX_PLAYERS 4
#define MAX_TRAIL 1000

#define B_HEIGHT 0
#define TRAIL_HEIGHT 3.5
#define CYCLE_HEIGHT 8
#define RECOGNIZER_HEIGHT 20
#define WALL_H 12

#define TURN_LENGTH 200

#define EXP_RADIUS_MAX 30
#define EXP_RADIUS_DELTA 0.01

/* these must be < 0 */
#define SPEED_CRASHED -1
#define SPEED_GONE -2

#define FAST_FINISH 40

#define PAUSE_GAME_FINISHED 1


#include "protocol.h"

extern char messages[80];

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

/* record.c */

extern void getEvents();
extern void writeEvent(GameEvent *e);
extern int startRecording(char *filename);
extern void stopRecording();
extern int startPlaying(char *filename);
extern void stopPlaying();

/* engine.c */

extern void addList(list **l, void *data);
extern int updateTime();
extern int getCol(int x, int y);
extern void turn(Data* data, int direction);
extern void idleGame();
extern void initGame();
extern void exitGame();
extern void initGameStructures();
extern void initGameScreen();
extern void defaultDisplay(int n);
extern void cycleDisplay(int p);
extern void doTrail(line *t, int value);
extern void fixTrails();
extern void clearTrails(Data *data);

extern void moveStep(Data* data);
extern void writePosition(int player);

extern void doTurn(Data* data, int time);
extern void crashPlayer(int player);
extern void createTurnEvent(int player, int direction);
extern int processEvent(GameEvent *e);

/* gltron.c */
/* game.c */
/* gamegraphics.c */
extern void initData();
extern int colldetect(float sx, float sy, float ex, float ey, int dir, int *x, int *y);

extern int allAI();
extern void initGlobals(void);

/* ai -> computer.c */

extern void doComputer(int player, int target);
extern void doComputer2(int player, int target);

extern void initMainGameSettings();
extern void initDefaultSettings();

/* demo stuff */
/* record.c */
extern int  readDemoInfo();
extern int  writeDemoInfo();
extern void resetScores();
extern int applyGameInfo();
extern void movePlayers();

/* server stuffs */
extern void initNetwork();
extern void start_server();
extern void stop_server();
extern void handle_server();
extern void SendEvents(GameEvent *e);
extern void do_wingame(int);
extern void do_timeout( void );
extern void do_ping_users( void );
extern void do_starting();

extern int getPlayer          ( int );
extern int getWhich           ( int );
extern int find_freeplayer     ( void );

extern TCPsocket         servsock;
extern SDLNet_SocketSet  socketset;
extern int               nbUsers;
extern int               sState;
extern Slots             slots[MAX_PLAYERS];
extern Netscores         netscores;

extern int server_port;
extern int hasstarted;

#endif


