/** Author: Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
    This describle the protocol used by the tracker.
*/
#ifndef __TRACKER_PROTOCOL_H__
#define __TRACKER_PROTOCOL_H__

//Enums-------------------------------------------------------------------
//active state of a slot
enum {
  INACTIVE,                 //this slot is not active
  NEGOCIATION,              //this slot is in negociation to be active
  ACTIVE                    //this slot is active
};

//type of a slot
enum {
  SERVER,                   //this slot is a connection to a server
  CLIENT                    //this slot is a connection to a client
};

//TypeDefs-----------------------------------------------------------------
//Server slots
typedef struct {
  int           active;       //Slot is active?
  TCPsocket     sock;         //client's socket
  IPaddress     peer;         //client's address
  int           type;         //type client or server.
} Trackerslots;


typedef enum tpackettype
  {
    TLOGIN,                   //Asking for login.
    TCLOSE,                   //A server has close
    TINFOS                    //information of a server
  } TpacketType;

//Tracker Packet
typedef struct {
  TpacketType   type;         //type du packet
  int           which;        //where does it come from
  union         tinfos        //Additional infos
  {
    struct {
      int      type;          //type
      char     passwd[9];     //passwd for login to tracker( not needed for clients )
      char     nick[9];       //Nickname to use in the game.
    } login;              //Type TLOGINREP
    struct {
      int      speed;         //speed of the game
      int      size;          //arena size
      int      erase;         //if trail are erased when cycle death
      char     description[32]; //description of the server
      char     version[9];    //which gltron's version is comptatible with
      IPaddress ipaddress;    //what is its ip and its port?
      int      nbplayers;     //how many players on that server?
    } infos;                  //Type TINFOS
  } infos;
} Trackerpacket;

//Prototypes------------------------------------------------------------------

/** new function specific to tracker, others still protocol's one */
int Net_treceivepacket    ( Trackerpacket *packet , TCPsocket sock, int which  );
int Net_tsendpacket       ( Trackerpacket *packet , TCPsocket sock             );
#endif
