
typedef struct scnetEventList *cnetEventList;
typedef struct scnetEventList {
  GameEvent       event;
  cnetEventList   next;
} tcnetEventList;

typedef struct snetEventList {
  cnetEventList head;
} tnetEventList, *netEventList;


/** Creates */
netEventList    createNetEventList ( void );
GameEvent*      getNetEvent        ( void );
void            addNetEvent        ( GameEvent* );

/** Getters */
int             getPlayer          ( int );
int             getWhich           ( int );

/** Inits */
void            initNetEventList    ( netEventList );
