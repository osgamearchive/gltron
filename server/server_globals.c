#include "server_gltron.h"

/* globals */
Game main_game;
Game *game;
Game2 main_game2;
Game2 *game2;

char messages[80];

unsigned char* colmap = NULL;
int colwidth;

int dirsX[] = { 0, -1, 0, 1 };
int dirsY[] = { -1, 0, 1, 0 };

// int default_arena_sizes[] = { 160, 240, 400, 600, 1200 };
int default_arena_sizes[] = { 160, 240, 480, 720, 1200 };
// int default_texture_modifier[] = { be 
float default_speeds[] = { 5.0, 6.5, 8.5, 12.0, 0.0 };

/** server globals */

//main server socket ( listening incoming connexion )
TCPsocket         servsock=NULL;
//SocketSet for locking every sockets.
SDLNet_SocketSet  socketset=NULL;
//NbUsers connected and logged.
int               nbUsers=0;
//Server state.
int               sState=-1; //-1 means server is down...
//Slots infos
tSslots           slots[MAX_PLAYERS];
//Net settins :: Ugly, change that...
netGameSettings   netSettings;
