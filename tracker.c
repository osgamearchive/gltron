#include "gltron.h"

static Trackerslots servers[MAX_SLOTS];
static Wlist       *serverlist = NULL;
static int nbservers = 0;

static char *speed_list[] = {  "boring", "normal", "fast", "crazy", NULL };
static char *arena_list[] = { "tiny", "medium", "big", "vast", "extreme", NULL };


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
  nbservers=0;
  //init pings
  init_ping(1);

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
  //char **cols;
  char  host[255];
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
  servers[which].ping=0;
  servers[which].packets=0;

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

/*   cols = (char**)malloc(sizeof(char*)*6); */
/*   for(i=0; i<6; ++i) */
/*     { */
/*       cols[i] = (char *) malloc(255); */
/*     } */

/*   sprintf(cols[0], "%d.%d.%d.%d:%d", */
/* 	 (ntohl(servers[which].ipaddress.host) & 0xff000000) >> 24, */
/* 	  (ntohl(servers[which].ipaddress.host) & 0x00ff0000) >> 16, */
/* 	 (ntohl(servers[which].ipaddress.host) & 0x0000ff00) >> 8, */
/* 	 ntohl(servers[which].ipaddress.host) & 0x000000ff, */
/* 	  servers[which].ipaddress.port); */
/*   sprintf(cols[1], "v:%d s:%d e:%d", servers[which].speed, */
/* 	  servers[which].size, servers[which].erase); */
/*   sprintf(cols[2], "%s", servers[which].description); */
/*   sprintf(cols[3], "%d", servers[which].nbplayers); */
/*   sprintf(cols[4], "%s", servers[which].version); */
/*   sprintf(cols[5], "%d", servers[which].ping); */


  sprintf(host, "%d.%d.%d.%d",
	  (ntohl(servers[which].ipaddress.host) & 0xff000000) >> 24,
	  (ntohl(servers[which].ipaddress.host) & 0x00ff0000) >> 16,
	  (ntohl(servers[which].ipaddress.host) & 0x0000ff00) >> 8,
	  ntohl(servers[which].ipaddress.host) & 0x000000ff);
  
  make_ping(which, servers, host, PINGPORT);
  nbservers++;
  i = addRow_wlist  ( serverlist, 1 );
  if( i >= 0 )
    {
      printf("--> setting line %d\n", i);
      setCell_wlist ( serverlist, (char *)&servers[which].ipaddress, sizeof(IPaddress), i, 0 );
      setCell_wlist ( serverlist, (char *)servers[which].description, strlen(servers[which].description)+1, i, 1 );
      setCell_wlist ( serverlist, (char *)&servers[which].nbplayers, sizeof(int), i, 2 );
      setCell_wlist ( serverlist, (char *)&servers[which].ping, sizeof(int), i, 3 );
    }
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
      //restoreCallbacks();
      trackeruse=0;
      //switchCallbacks(&guiCallbacks);
      changeCallback(&guiCallbacks, &guiCallbacks);
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

static int current = -1;

void
displayServerLegend()
{
  float color[3] =  { 0.6, 0.6, 0.6 };
  int x, y;
  int h;
  char str[255];

  if( current == -1 )
    return;

  h = game->screen->vp_h / (24 * 1.5);
  h+=2;
  glColor3fv(color);
  x = game->screen->vp_w/2 - 1.5 * 14 *( game->screen->vp_w / (50 * 1.5) );
  //y = game->screen->vp_h - 1.5 * h;
  y = game->screen->vp_h - 50;
  sprintf(str, "Server       : %d.%d.%d.%d",
	 (ntohl(servers[current].ipaddress.host) & 0xff000000) >> 24,
	  (ntohl(servers[current].ipaddress.host) & 0x00ff0000) >> 16,
	 (ntohl(servers[current].ipaddress.host) & 0x0000ff00) >> 8,
	 ntohl(servers[current].ipaddress.host) & 0x000000ff);
  drawText(gameFtx, x, y, h, str);
  y = game->screen->vp_h - 70;

  sprintf(str, "Version      : %s",servers[current].version);
  drawText(gameFtx, x, y, h, str);

 sprintf(str, "Speed        : %s",speed_list[servers[current].speed]);
  y = game->screen->vp_h - 90;
  drawText(gameFtx, x, y, h, str);

  sprintf(str, "Size         : %s", arena_list[servers[current].size]);
  y = game->screen->vp_h - 110;
  drawText(gameFtx, x, y, h, str);
}

void
displayTrackerScreen()
{
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0, 0.5, 0.5} , { 0.0, 0.5, 1.0 }};
  int x, y;
  int h;
  char str[255];

  rasonly(game->screen);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //draw Title
  h = game->screen->vp_h / (24 * 1.5);


  glColor3fv(colors[0]);
  x = game->screen->vp_w/2 - 1.5 * 7 *( game->screen->vp_w / (50 * 1.5) );
  y = game->screen->vp_h - 1.5 * h;
  drawText(gameFtx, x, y, h+3, "GLTRON SERVERS");
  

  //draw number of servers
  y = 1.5 * h;
  glColor3fv(colors[1]);
  sprintf(str, "%d gltron servers.", nbservers);
  x = game->screen->vp_w/2 - 1.5 * strlen(str)/2 *( game->screen->vp_w / (50 * 1.5) );
  drawText(netFtx, x, y, h, str);

  //draw server info
  displayServerLegend();

  //draw server list
  draw_wlist(serverlist);
  SystemSwapBuffers();
}

void
idleTracker()
{
  int sockstat = socksnotready;
  int i;
  //float pingf;
  //short *ping=malloc(sizeof(short));
  int ping;
  //char str[255];

  sockstat = Net_checksocket(Net_gettrackersock());

  while(  ready_ping() )
    {
      handle_ping(servers);


      for(i=0; i<nbservers; ++i)
	{
	  printf("%d %d\n", servers[i].ping, servers[i].packets);
	  if( servers[i].packets > 0 )
	    {
	      printf("new ping is %d ( %d )\n", servers[i].ping, servers[i].packets);
	      
	      //sprintf(str, "%d", servers[i].ping/servers[i].packets);
	      ping = (servers[i].ping/servers[i].packets)*1;
	      //*ping = (int)(pingf);
	      //(*ping)=(int)(pingf)*1;
	      printf("ping is %d\n", ping);
	      /*********************************************************v****/
	      setCell_wlist ( serverlist, (char *)&ping, sizeof(int), i, 3 );
	      rebuildindex_wlist(serverlist);
	    }
	}	  
    }
  //free(ping);

  if( sockstat != socksnotready )
    {
      if( sockstat & tcpsockready )
	{
	  tracker_handle();
	}
    }
  SystemPostRedisplay();
}

void
keyTracker(int k, int unicode, int x, int y)
{
  //char *str = NULL;
  char server[255], port[255];
  IPaddress *ipaddress=NULL;
  //char *server=NULL, *port=NULL;

  switch(k)
    { 
    case SYSTEM_KEY_F11: doBmpScreenShot(); break;
    case SYSTEM_KEY_F12: doScreenShot(); break;
    case SDLK_ESCAPE:
      tracker_close();
      free_wlist(serverlist);
      serverlist=NULL;
      nbservers=0;
      //restoreCallbacks();
      trackeruse=0;
      //switchCallbacks(&guiCallbacks);      
      changeCallback(&guiCallbacks, &guiCallbacks);
      break;
    case SDLK_UP:
      scroll_wlist(serverlist, LIST_SCROLL_UP);
      break;
    case SDLK_DOWN:
      scroll_wlist(serverlist, LIST_SCROLL_DOWN);
      break;
    case 13:
      //connect to the server
      //str = getcell_wlist ( serverlist, getcurrent_wlist(serverlist), 0);
      ipaddress = (IPaddress *)getCell_wlist( serverlist, getcurrent_wlist(serverlist), 0);

      if( ipaddress == NULL )
	{
	  fprintf(stderr, "error ipaddress null\n");
	  return;
	}

      //getting server address
      sprintf(server, "%d.%d.%d.%d", 
	      (ntohl(ipaddress->host) & 0xff000000) >> 24,
	      (ntohl(ipaddress->host) & 0x00ff0000) >> 16,
	      (ntohl(ipaddress->host) & 0x0000ff00) >> 8,
	      ntohl(ipaddress->host) & 0x000000ff);

      //getting port
      sprintf(port, "%d", ipaddress->port);

      if( server == NULL )
	return;
      printf("server %s\n", server);
      //strcpy(server,"");
      // sscanf(str, "%[.0-9]:%[.0-9]", server, port);
/*       strcpy(game->settings->server, server); */
/*       strcpy(game->settings->port, port); */
      //if( !strcmp(server, "") )
      //if(server == NULL )
      //	return;
      setconnection(server, port);
      isConnected=0;
      isLogged=0;
      printf("server %s port %s\n", server, port);
      tracker_close();
      free_wlist(serverlist);
      serverlist=NULL;      
      changeCallback(&netConnectCallbacks, &trackerCallbacks);
      //switchCallbacks(&netConnectCallbacks);
      break;
    }
}


void
drawit( WlistPtr list, int x, int y, int line, int col )
{
  int h = serverlist->height/serverlist->nblines;
  int s = h-15;

  glColor3f(serverlist->colDefs[col].color[0], serverlist->colDefs[col].color[1], serverlist->colDefs[col].color[2] );
  //fprintf(stderr, "line %d col %d\n", line, col);
  //fprintf(stderr, "%s\n", serverlist->lines[line][col]);
  drawText(netFtx, x, y-h/2+7, s, serverlist->lines[line][col]);
}

char *
addressToStr( WlistPtr list, int line, int col )
{
  char *str = malloc(255);
  IPaddress *ipaddress;

  ipaddress = ( IPaddress *)list->lines[line][col];
  if( ipaddress == NULL )
    {
      strcpy(str, "N/A");
    } else {

      sprintf(str, "%d.%d.%d.%d",
	      (ntohl(ipaddress->host) & 0xff000000) >> 24,
	      (ntohl(ipaddress->host) & 0x00ff0000) >> 16,
	      (ntohl(ipaddress->host) & 0x0000ff00) >> 8,
	      ntohl(ipaddress->host) & 0x000000ff
	      );
    }
  return str;
}

char *
intToStr( WlistPtr list, int line, int col )
{
  char *str = malloc(255);
  int *val;

  val = (int *)(list->lines[line][col]);
  if( val == NULL )
    {
      strcpy(str, "N/A");
    } else {

      //val = (int)(*(list->lines[line][col]));

      sprintf(str, "%d", *val);
    }
  return str;
}


char *
charToStr(WlistPtr list, int line, int col )
{
  char *str = malloc(255);

  if(  list->lines[line][col] == NULL )
    {
      strcpy(str, "N/A");
    } else {
      sprintf(str, "%s", list->lines[line][col]);
    }
  return str;
}

void
focus(WlistPtr list, int line)
{
  current = line;
  printf("focus on server %d\n", line);
}


int
sortit( WlistPtr list, int line, int next )
{
  int *a;
  int *b;

  a = (int *)(list->lines[line][list->sortcol]);
  b = (int *)(list->lines[next][list->sortcol]);

  if( a == NULL || b == NULL )
    return 0;
  printf(" comparing %d (%d) and %d (%d) %d\n", *a,line, *b,next, (*a > *b));
  return (*a > *b);
}

void
initTracker()
{
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0, 0.5, 0.5} , { .4, 0.9, .4 }};
  ColDef *colDefs;

  if( serverlist != NULL )
    return;

  colDefs = new_colDefs( 4 );

  set_colDef( colDefs, 0, "Address", 30, colors[1], NULL, addressToStr, NULL); 
  set_colDef( colDefs, 1, "Description", 40, colors[1], NULL, charToStr, NULL); 
  set_colDef( colDefs, 2, "Players", 20, colors[1], NULL, intToStr, NULL); 
  set_colDef( colDefs, 3, "Ping", 10, colors[3], NULL, intToStr, sortit); 

  serverlist = new_wlist(10, 60,game->screen->vp_w-20, game->screen->vp_h-250,
			 10, 4, colDefs, 3, focus);
  trackeruse=1;
  current=-1;
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

