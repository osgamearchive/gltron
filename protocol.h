/** Author: Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
    This describle the protocol used by gltron to talk with the server.
*/

//Defines---------------------------------------------------------------------
#define SERVERID   -1        //This is the server id

#define BROADCAST  -1        //Used when chat is not private.

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
  PART,                     //Someone left
  JOIN                      //Someone join
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
    NETRULES,                //Net rules: rules that set the server behaviour.
    SCORE,                   //Score at the end of a game.
    SNAPSHOT,                //Snapshot ( quite same as EVENT )
    EVENT,                   //Event
    ACTION                   //This is a simple action.
  } PacketType;

/** the Packet him self using union */
typedef struct packet {
  PacketType   type;         //The type of the packet.
  int          which;        //Who send the packet.
  union        infos         //Additional infos
  {
    struct {
      char     version[255]; //Version of gltron.
      char     nick[255];    //Nickname to use in the game.
    } login;                 //Type LOGIN
    struct {
      int      accept;       //1 if login accepted, else 0.
      char     message[255]; //A message from the server.
    } loginrep;              //Type LOGINREP
    struct {
      int      which;        //who are we talking about?
      int      ismaster;     //Is he the game master?
      int      color;        //server decide players's color.
      char     nick[255];    //His nickname.
    } userinfo;              //Type USERINFO
    struct {
      int      serverstate;  //The server state.
      int      players;      //Who many players are connected.
    } serverinfo;            //Type SERVERINFO
    struct {
      char     mesg[1024];   //The message
      int      which;        //destination of the message.
    } chat;                  //Type CHAT
    struct {
      int      players;      //how many players
      float    speed;        //game speed
      int      eraseCrashed; //1 if erase Crashed player.
      int      startPos[MAX_PLAYERS*3]; //startposition of players
      Time     time;         //ot synchronize
      int      gamespeed;    //The game speed
      int      grid_size;    //grid size
      int      arena_size;   //arena_size
    } gamerules;             //Type GAMERULES
    struct {
      int      nbWins;       //NbWins before getting score
      int      time;         //How many time before stopping game.
    } netrules;
    struct {
      int     winner;        //Who is the winner
      int     points[MAX_PLAYERS]; //Points for each player.
    } score;
    struct {
      GameEvent events[4];   //4 events
    } snapshot;              //Type SNAPSHOT
    struct {
      GameEvent event;       //event
    } event;                 //Type EVENT
    struct {
      int type;              //Which action?
      int which;             //To which if necessary.
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
  int           isMaster;    //If it's a master
} Slots;

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
int       Net_receivepacket      ( Packet *, TCPsocket );