#include "gltron.h"

static Trackerslots servers[MAX_SLOTS];
static Wlist       *serverlist = NULL;

int
tracker_connect()
{
  int port = strtol(game->settings->tport, (char**) NULL, 10); 
  Trackerpacket packet;

  if( Net_tconnect(game->settings->tracker, port) )
    {
      fprintf(stderr, "can't connect to the tracker! %s\n", SDLNet_GetError());
      return 1;
    }
  printf("connected to the tracker\n");

  Net_allocsocks();
  Net_addsocket(Net_gettrackersock());

  //init pings
  init_ping();

  //login
  packet.type=TLOGIN;
  packet.which=0;
  packet.infos.login.type=CLIENT;
  Net_tsendpacket(&packet, Net_gettrackersock());
  return 0;
}


void
tracker_close()
{
  Net_tdisconnect();
  Net_delsocket(Net_gettrackersock());
  close_ping();
  Net_freesockset();
}


void
tracker_infos(Trackerpacket *packet)
{
  int which = packet->which;
  char **cols;
  //char *cols[6];
  int i;

  //here we get the information from a server

  //saving informations
  servers[which].speed=packet->infos.infos.size;
  servers[which].size=packet->infos.infos.size;
  servers[which].erase=packet->infos.infos.erase;
  strcpy(servers[which].description, packet->infos.infos.description);
  strcpy(servers[which].version, packet->infos.infos.version);
  servers[which].ipaddress=packet->infos.infos.ipaddress;
  servers[which].nbplayers=packet->infos.infos.nbplayers;
  
  
  printf("------------------------------------------\n");
  printf("getting server infos %d \n", which);
  printf("speed       : %d\n",servers[which].speed ); 
  printf("size        : %d\n",servers[which].size ); 
  printf("erase       : %d\n",servers[which].erase ); 
  printf("description : %s\n",servers[which].description ); 
  printf("version     : %s\n",servers[which].version );  

  #ifndef macintosh /* no ntohl on MacOS */
  printf("IP          : %d.%d.%d.%d:%i\n",
	 (ntohl(servers[which].ipaddress.host) & 0xff000000) >> 24,
	  (ntohl(servers[which].ipaddress.host) & 0x00ff0000) >> 16,
	 (ntohl(servers[which].ipaddress.host) & 0x0000ff00) >> 8,
	 ntohl(servers[which].ipaddress.host) & 0x000000ff,
	 servers[which].ipaddress.port
 ); 
  #endif
  printf("nbplayers   : %d\n",servers[which].nbplayers ); 
  printf("------------------------------------------\n");

  cols = (char**)malloc(sizeof(char*)*6);
  for(i=0; i<6; ++i)
    {
      cols[i] = (char *) malloc(255);
    }

  sprintf(cols[0], "%d.%d.%d.%d:%d",
	 (ntohl(servers[which].ipaddress.host) & 0xff000000) >> 24,
	  (ntohl(servers[which].ipaddress.host) & 0x00ff0000) >> 16,
	 (ntohl(servers[which].ipaddress.host) & 0x0000ff00) >> 8,
	 ntohl(servers[which].ipaddress.host) & 0x000000ff,
	  servers[which].ipaddress.port);
  sprintf(cols[1], "v:%d s:%d e:%d", servers[which].speed,
	  servers[which].size, servers[which].erase);
  sprintf(cols[2], "%s", servers[which].description);
  sprintf(cols[3], "%d", servers[which].nbplayers);
  sprintf(cols[4], "%s", servers[which].version);
  sprintf(cols[5], "%d", servers[which].ping);

  newline_wlist(serverlist, cols);
}

void
tracker_handle()
{
  Trackerpacket packet;
  
  //Get the packet...
  if( Net_treceivepacket(&packet, Net_gettrackersock()) != 0 )
    {
      //Connection perdu
      fprintf(stderr, "connection lost\n");
      tracker_close();
      switchCallbacks(&guiCallbacks);
      return;
    }

  switch( packet.type )
    {
    case TINFOS:
      tracker_infos(&packet);
      break;
    default:
      fprintf(stderr, "unknown packet from tracker\n");
      break;
    }
}

void
displayTrackerScreen()
{
  rasonly(game->screen);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_wlist(serverlist);
  SystemSwapBuffers();
}

void
idleTracker()
{
  int sockstat = socksnotready;

  sockstat = Net_checksocket(Net_gettrackersock());
  if( sockstat != socksnotready )
    {
      if( sockstat & tcpsockready )
	{
	  tracker_handle();
	}
      if( sockstat & udpsockready )
	{
	  handle_ping(servers);
	}
    }
  SystemPostRedisplay();
}

void
keyTracker(int k, int unicode, int x, int y)
{
  char *str;
  char server[255], port[255];

  switch(k)
    { 
    case SYSTEM_KEY_F11: doBmpScreenShot(); break;
    case SYSTEM_KEY_F12: doScreenShot(); break;
    case SDLK_ESCAPE:
      tracker_close();
      free_wlist(serverlist);
      serverlist=NULL;
      switchCallbacks(&guiCallbacks);
      break;
    case SDLK_UP:
      scroll_wlist(serverlist, LIST_SCROLL_UP);
      break;
    case SDLK_DOWN:
      scroll_wlist(serverlist, LIST_SCROLL_DOWN);
      break;
    case 13:
      //connect to the server
      str = getcell_wlist ( serverlist, getcurrent_wlist(serverlist), 0);
      printf("server %s\n", str);
      sscanf(str, "%[.0-9]:%[.0-9]", server, port);
/*       strcpy(game->settings->server, server); */
/*       strcpy(game->settings->port, port); */
      setconnection(server, port);
      isConnected=0;
      isLogged=0;
      printf("server %s port %s\n", server, port);
      tracker_close();
      free_wlist(serverlist);
      serverlist=NULL;
      switchCallbacks(&netConnectCallbacks);
      break;
    }
}

void
initTracker()
{
  char *titles[] = {"server", "settings", "description", "players", "version", "ping"};
  if( serverlist == NULL )
    serverlist = new_wlist(10, 10,game->screen->vp_w-20, game->screen->vp_h-20,
			   10, 6, titles);
}

void
mouseTracker(int buttons, int state, int x, int y)
{

}

void
cleanTracker()
{

}

callbacks trackerscreenCallbacks = {
  displayTrackerScreen, idleTracker, keyTracker, initTracker,
  cleanTracker, NULL, mouseTracker, NULL
};
