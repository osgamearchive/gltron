/** Author: Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr ) */

#ifndef __PROTOCOLE__
#define __PROTOCOLE__

//Defines---------------------------------------------------------------
#define    loginAccept  1  //Login accepté
#define    loginRefuse  2  //Login refusé
#define    observer     3  //Etat d'observer
#define    participate  4  //Etat de participant
#define    waitState    5  //Etat d'attente
#define    preGame      6  //Etat de prejeux
#define    startGame    7  //Démarrage de la partie
#define    stopGame     8  //Arret de la partie
#define    pauseGame    9  //Mise en pause de la partie
#define    resumeGame   10 //Arret de la pause
#define    snapshot     11 //Envoi d'un snapShot
#define    joinPlayer   12 //Un joueur vient de rejoindre
#define    leftPlayer   13 //Un joueur vient de quitter
#define    turnRight    14 //Un joueur vient de tourner à droite
#define    turnLeft     15 //Un joueur vient de tourner à gauche
#define    crash        16 //Un joueur vient de se crasher
#define    ping         17 //Résultat d'un ping
#define    quitGame     18 //Le joueur quitte le jeu
#define    chat         19 //c du chat...
#define    gameEvent    20 //C un event du jeu
#define    chgeState    21 //Changement d'état du serveur
#define    preGameState 22 //Etat de preGame
#define    gameState    23 //partie commencée


#define SERVER_PORT     23460
#define SERVERID        -1

#define magic           255

//Status
#define connected       255

//Erreurs
#ifndef noErr
#define noErr           0
#endif


#define connectClosed    -1
#define connectLost      -2
#define cantfindhost     -3
#define cantconnect      -4
#define cantallocsockset -5
#define corruptedPacket  -6
#define loginNotAccepted -7
#define unkownError      -254


//Typedefs-------------------------------------------------------------------------------------
typedef struct sWelcom
{
  char       name[255];  //Nom du gars qui se connecte
  int        vers;       //Version de gltron ( pour les comptatibilités )
} tWelcom, *pWelcom;

typedef struct sServRepHdr
{
  int which;             //De qui vient la réponse ( -1 si c le serveur )
  int type;              //Type de la réponse
  int len;               //Longueur de la réponse
  int time;              //temps
} tServRepHdr, *pServRepHdr ;


typedef struct sLogin
{
  int state;             //Etat du server
  int nbUsers;           //Nombre d'utilisateurs déjà connecté

} tLogin, *pLogin;

typedef struct sWho
{
  int  which;            //De qui on parle
  int  pingIt;           //Le ping du gars
  int  color;            //Sa couleur
  int  isMaster;         //Si c le maître de la partie
  char name[255];        //Nom du gars
}  tWho, *pWho;

typedef struct sslots {
  int      active;       //Le slot est il actif?
  Uint8    name[255];    //Nom de l'utilisateur
  int      color;        //Couleur de la moto
  int      pingIt;       //Ping de l'utilisateur
  int      isMaster;     //Est ce le maitre du jeu
} tslots, *pslots;


typedef struct snetGameSettings {
  Grid    grid;          //taille de l'art
  RuleSet rule;          //vitesse...
  int     timeR;         //Le temps de la partie, si 0 illimité.
  int     nbGames;        //nbre de parties par jeu.
} tnetGameSettings, *netGameSettings;

//Prototypes

/** Fonctions Generales */
void Net_init            ( void );
void Net_cleanup         ( void );
int  Net_connect         ( char *, int);
void Net_deconnect       ( void );
int  Net_allocSocks      ( void );
int  Net_checkSocks      ( void );
int  Net_SockisValid     ( void );

/** Sends */
int  Send_welcom         ( char*, int );
int  Send_header         ( int, int, int, int );
int  Send_login          ( int, int );
int  Send_who            ( int, int, int, int, char* );
int  Send_chat           ( char *);
int  Send_Buff           ( char *);
int  Send_chgeState      ( int );

/** Recvs */
int  Recv_header         ( int *, int *, int *, int * );
int  Recv_login          ( int *, int * );
int  Recv_who            ( pslots ); 
int  Recv_chat           ( int, char *);
//char* Recv_chat( int  );
void Recv_Buff           ( char *, int );

GameEvent* Recv_gameEvent();
int  Recv_chgeState      ( int * );
int  Recv_netGameSettings( netGameSettings );

/** Creates */
netGameSettings createNetGameSettings  ( void );
void            defaultNetGameSettings ( netGameSettings );
/** Prints */
void print_serverState   ( int );
void printNetGameSettings ( netGameSettings );

#endif
