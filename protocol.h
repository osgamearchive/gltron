/** Author: Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
    This describle the protocol used by gltron to talk with the server.
*/

//Defines---------------------------------------------------------------------
#define SERVERID   -1        //This is the server id

#define BROADCAST  -1        //Used when chat is not private.

#define PACKETSIZE  56       //Size of biggest packet

//Enums-----------------------------------------------------------------------
/** Possible errors */
enum {
  connectionclosed =  -1,    //The connection has been closed.
  connectionlost   =  -2,    //Same as connectionClosed, but we don't know why!
  cantfindhost     =  -3,    //We can't find host!
  cantconnect      =  -4,    //Can't connect means connection refused.
  cantallocsockset =  -5,    //We can't allocate a SocketSet.
  corruptedpacket  =  -6,    //means a paquet is not as we expected...
  loginnotaccepted =  -7,    //login was refused for a reason...
  cantsendpacket   =  -8,    //Cant send a packet...
  cantgetpacket    =  -9,    //Can't get a packet
  unknownerror     = -255    //Unknown Error...
};

/** Server state */
enum {
  waitState,                 //Server has init all, and wait his 1st player
  preGameState,              //Server wait for players and to start game
  gameState                  //Game is running
};

/** Actions */
enum {
  TURNRIGHT,                //Ask to turn right
  TURNLEFT,                 //Ask to turn left
  STARTGAME,                //Ask to start game
  CONFSTART,                //Game Master confirm start of the game
  PART,                     //Someone left
  JOIN,                     //Someone join
  CHGEGAMEMASTER,           //One server say that game master has changed
  CHGENBWINS,               //GameMaster ask to change nbWins setting
  CHGETIMEOUT,              //change time out, 0 is no timeout.
  CHGESPEED,                //Change game_speed
  CHGESIZE,                 //Change arena_size
  CHGEERASE,                //Change erased trail when crashed 1 if yes.
  HASSTARTED                //Game already started!
};

//Typedefs--------------------------------------------------------------------

/** PacketType  */
typedef enum packettype
  {
    LOGIN,                   //Asking for login.
    LOGINREP,                //The answer.
    USERINFO,                //Infos of a user.
    SERVERINFO,              //Infos of the server.
    CHAT,                    //Chat public and private.
    GAMERULES,               //Game rules and settings.
    STARTPOS,                //Start positions of the players ( server decide ).
    NETRULES,                //Net rules: rules that set the server behaviour.
    GAMESET,                 //Just to show what is current speed and size.
    SCORE,                   //Score at the end of a game.
    SNAPSHOT,                //Snapshot ( quite same as EVENT )
    EVENT,                   //Event
    ACTION                   //This is a simple action.
  } PacketType;

enum {
    HEADER  = 255            //This is an header, say type of next packet
};

/** the Packet him self using union */
typedef struct packet {
  PacketType   type;         //The type of the packet.
  Sint16       which;        //Who send the packet.
  union        infos         //Additional infos
  {
    struct {
      char     version[9];   //Version of gltron.
      char     nick[9];      //Nickname to use in the game.
    } login;                 //Type LOGIN
    struct {
      int      accept;       //1 if login accepted, else 0.
      char     message[32];  //A message from the server.
    } loginrep;              //Type LOGINREP
    struct {
      Sint16   which;        //who are we talking about?
      Sint16   ismaster;     //Is he the game master?
      Sint16   color;        //server decide players's color.
      char     nick[9];      //His nickname.
    } userinfo;              //Type USERINFO
    struct {
      Sint16   serverstate;  //The server state.
      Sint16   players;      //Who many players are connected.
    } serverinfo;            //Type SERVERINFO
    struct {
      char     mesg[32];     //The message
      Sint16   which;        //destination of the message.
    } chat;                  //Type CHAT
    struct {
      Sint16   players;      //how many players
      float    speed;        //game speed
      Sint16   eraseCrashed; //1 if erase Crashed player.
      Time     time;         //to synchronize
      Sint16   gamespeed;    //The game speed
      Sint16   grid_size;    //grid size
      Sint16   arena_size;   //arena_size
    } gamerules;             //Type GAMERULES
    struct {
      Sint16   startPos[MAX_PLAYERS*3]; //startposition of players
    } startpos;              //Type STARTPOS
    struct {
      Sint16   nbWins;       //NbWins before getting score
      Sint16   time;         //How many time before stopping game.
    } netrules;
    struct {
      Sint16   eraseCrashed; //1 if erase Crashed player.
      Sint16   gamespeed;    //The game speed
      Sint16   arena_size;   //arena_size      
    } gameset;
    struct {
      Sint16   winner;        //Who is the winner
      Sint16   points[MAX_PLAYERS]; //Points for each player.
    } score;
    struct {
      GameEvent event;       //events
    } snapshot;              //Type SNAPSHOT
    struct {
      GameEvent event;       //event
    } event;                 //Type EVENT
    struct {
      Sint16    type;        //Which action?
      Sint16    which;       //To which if necessary.
    } action;
  } infos;
} Packet;

//Server slots
typedef struct {
  int           active;      //Slot is active?
  TCPsocket     sock;        //client's socket
  IPaddress     peer;        //client's address
  char          name[255];   //nickname of the client
  int           points;      //points of the player
  int           color;       //color of the player
  int           player;      //Equiv player ( for the client )
  int           packet;      //Next Packet type
  int           isMaster;    //If it's a master
} Slots;

typedef struct {
  int          winner;      //Who win? -1 if game not finished
  int          points[MAX_PLAYERS]; //Player's pioints.
} Netscores;
//Globals---------------------------------------------------------------------
extern int isConnected;
extern int netrulenbwins;
extern int netruletime;

//Prototypes------------------------------------------------------------------

/** Core functions */
void      Net_init               ( void );
void      Net_cleanup            ( void );
int       Net_connect            ( char*, int );
void      Net_disconnect         ( void );
void      Net_closesock          ( TCPsocket );
int       Net_allocsocks         ( void );
int       Net_readysock          ( TCPsocket );
int       Net_checksocks         ( void );
int       Net_addsocket          ( TCPsocket );
int       Net_delsocket          ( TCPsocket );
TCPsocket Net_getmainsock        ( void );

/** Send a packet */
int       Net_sendpacket         ( Packet* , TCPsocket );
/** Receive a packet */
int       Net_receivepacket      ( Packet *, TCPsocket, int, int );
