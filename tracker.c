#include "gltron.h"

static Trackerslots servers[MAX_SLOTS];
static Wlist       *serverlist = NULL;
static WrootControl *trackerControls = NULL;
static int nbservers = 0;

static char *speed_list[] = {  "boring", "normal", "fast", "crazy", NULL };
static char *arena_list[] = { "tiny", "medium", "big", "vast", "extreme", NULL };

#define PINGLIMIT 400

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
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0, 0.5, 0.5} , { .8, .1, .1 }};
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
  /* drawText(gameFtx, x, y, h+3, "GLTRON SERVERS"); */
  
  //draw server info
  /* displayServerLegend(); */

  //draw server list
  //draw_wlist(serverlist);
  updateControls(trackerControls);

  //Display errors
  y = 1.5 * h * 2;
  glColor3fv(colors[3]);
  if(getcurrent_wlist(serverlist)>=0 )
    {
      if( servers[getcurrent_wlist(serverlist)].packets > 0 && ((servers[getcurrent_wlist(serverlist)].ping/servers[getcurrent_wlist(serverlist)].packets) > PINGLIMIT))
	{
	  if( strcmp(servers[getcurrent_wlist(serverlist)].version, VERSION))
	    {
	      sprintf(str, "Your version is not comptatible and ping is too high" );
	      x = game->screen->vp_w/2 - 1.5 * strlen(str)/2 *( game->screen->vp_w / (50 * 1.5) );
	      drawText(netFtx, x, y, h-1, str);
	    } else {
	      sprintf(str, "ping is too high");
	      x = game->screen->vp_w/2 - 1.5 * strlen(str)/2 *( game->screen->vp_w / (50 * 1.5) );
	      drawText(netFtx, x, y, h-1, str);
	    }
	} else {
	  if( strcmp(servers[getcurrent_wlist(serverlist)].version, VERSION))
	    {
	      sprintf(str, "Your version is not comptatible");
	      x = game->screen->vp_w/2 - 1.5 * strlen(str)/2 *( game->screen->vp_w / (50 * 1.5) );
	      drawText(netFtx, x, y, h-1, str);
	    }
	}
    }


  //draw number of servers
  y = 1.5 * h;
  glColor3fv(colors[1]);
  sprintf(str, "%d gltron servers.", nbservers);
  x = game->screen->vp_w/2 - 1.5 * strlen(str)/2 *( game->screen->vp_w / (50 * 1.5) );
  drawText(netFtx, x, y, h, str);
  drawMouse();

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
  switch(k)
    { 
    case SYSTEM_KEY_F11: doBmpScreenShot(); break;
    case SYSTEM_KEY_F12: doScreenShot(); break;
    case SDLK_ESCAPE:
      tracker_close();
      freeRootControl(trackerControls);
      trackerControls=NULL;
      serverlist=NULL;
      nbservers=0;
      //restoreCallbacks();
      trackeruse=0;
      //switchCallbacks(&guiCallbacks);      
      changeCallback(&guiCallbacks, &guiCallbacks);
      break;

    default:
      keyControls(trackerControls, k);
      break;
    }
}


void
drawit( WlistPtr list, int x, int y, int line, int col )
{
  int h = list->height/serverlist->nblines;
  //int s = h-15;


  //printf("comparing %d <>100 and %s <> %s\n", servers[line].ping, servers[line].version, VERSION);
  if( ( servers[line].packets > 0 && ((servers[line].ping/servers[line].packets) <= PINGLIMIT) && ! (strcmp(servers[line].version, VERSION)) )|| line > nbservers)
    return;

  //draw not available
  glColor3f(.3, .3, .4);
  glBegin(GL_QUADS);	  
  glVertex3f(x+1, y+h/2+1, 0.0f);     //top left
  if( col == list->nbcols-1 )
    {
      glVertex3f(x+ list->width*list->colDefs[col].colsize/100-1, y+h/2+1, 0.0f);   //top right
      glVertex3f(x+ list->width*list->colDefs[col].colsize/100-1, y-h/2-1, 0.0f);   //Bottom right
    } else {
      glVertex3f(x+ list->width*list->colDefs[col].colsize/100+1, y+h/2+1, 0.0f);   //top right
      glVertex3f(x+ list->width*list->colDefs[col].colsize/100+1, y-h/2-1, 0.0f);   //Bottom right
    }
  glVertex3f(x+1, y-h/2-1, 0.0f);         //Bottom left
  glEnd();


  //glColor3f(list->colDefs[col].color[0], list->colDefs[col].color[1], list->colDefs[col].color[2] );
  /* glColor3f(.8, .4, .4); */
/*   glBegin(GL_LINES); */
/*   glVertex2d(x,                                            y-h/2); */
/*   glVertex2d(x+(list->width*list->colDefs[col].colsize/100),   y+h/2); */
/*   glEnd(); */
/*   glBegin(GL_LINES); */
/*   glVertex2d(x,                                            y+h/2); */
/*   glVertex2d(x+(list->width*list->colDefs[col].colsize/100),   y-h/2); */
/*   glEnd(); */


  //fprintf(stderr, "line %d col %d\n", line, col);
  //fprintf(stderr, "%s\n", serverlist->lines[line][col]);
  //drawText(netFtx, x, y-h/2+7, s, serverlist->lines[line][col]);
  
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

void
mousefocus(WlistPtr list, int line, Wpoint mousexy)
{
  float color[4] = { 0.05f, 0.05f, 0.05f, .8f };
  int   x, y;
  int   width    = 400;
  int   h        = list->height/list->nblines;
  int   l        = (list->y+list->height-mousexy.v)/h;
  int   s        = 14;
  int   height   = s*6;
  char  str[255];

  //draw a beautifull transparent box using alpha trucs
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv(color);

  //find pos x, y
  x = list->x + list->width/2 - width/2;
  y = list->y+list->height-((l+1)*h)+0.5*h+(0.25*height)-1;//+((list->y+list->height-mousexy.v)/h)*h;

  glBegin(GL_QUADS); 
  glVertex3f(x, y, 0.0f);   //top left
  glVertex3f(x+width, y, 0.0f);   //top right
  glVertex3f(x+width, y-height, 0.0f);  //Bottom right
  glVertex3f(x, y-height, 0.0f);  //Bottom left  
  glEnd();
  glDisable(GL_BLEND);

  glColor3f(0.9,.3, .3);
  //draw text now
  y -=2*s;
  x+=10;
  sprintf(str, "Server       : %d.%d.%d.%d",
	  (ntohl(servers[line].ipaddress.host) & 0xff000000) >> 24,
	  (ntohl(servers[line].ipaddress.host) & 0x00ff0000) >> 16,
	  (ntohl(servers[line].ipaddress.host) & 0x0000ff00) >> 8,
	  ntohl(servers[line].ipaddress.host) & 0x000000ff);
  //x=list->x+list->width/2-strlen(str)*s*1.5/2;
  drawText(gameFtx, x, y, s, str);
  y -=s;

  glColor3f(.1,.9, .1);
  sprintf(str, "Version      : %s",servers[line].version);
  //x=list->x+list->width/2-strlen(str)*( game->screen->vp_w / (50 * 1.5) )*1.5/2;
  drawText(gameFtx, x, y, s, str);
  y -=s;

  glColor3f(.4,.4, .9);
  sprintf(str, "Speed        : %s",speed_list[servers[line].speed]);
  //x=list->x+list->width/2-strlen(str)*( game->screen->vp_w / (50 * 1.5) )*1.5/2;
 //y = game->screen->vp_h - 90;
  drawText(gameFtx, x, y, s, str);

  y -=s;
  sprintf(str, "Size         : %s", arena_list[servers[line].size]);
  //x=list->x+list->width/2-strlen(str)*( game->screen->vp_w / (50 * 1.5) )*1.5/2;
  //y = game->screen->vp_h - 110;
  drawText(gameFtx, x, y, s, str);
}


void
action(WlistPtr list)
{
  char server[255], port[255];
  IPaddress *ipaddress=NULL;

     if(  (servers[getcurrent_wlist(list)].packets > 0 && ((servers[getcurrent_wlist(list)].ping/servers[getcurrent_wlist(list)].packets) > PINGLIMIT)) || strcmp(servers[getcurrent_wlist(list)].version, VERSION))
	return;

      //str = getcell_wlist ( list, getcurrent_wlist(list), 0);
      ipaddress = (IPaddress *)getCell_wlist( list, getcurrent_wlist(list), 0);

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
      freeRootControl(trackerControls);
      trackerControls=NULL;
      serverlist=NULL;      
      changeCallback(&netConnectCallbacks, &trackerCallbacks);
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
  int x, y, h;

 //draw Title
  h = game->screen->vp_h / (24 * 1.5);


  


  glShadeModel(GL_FLAT);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  trackeruse=1;

  if(  trackerControls != NULL )
    return;

  if(  serverlist != NULL )
    return;

  trackerControls = newRootControl();

  colDefs = new_colDefs( 4 );

  set_colDef( colDefs, 0, "Address", 30, colors[1], drawit, addressToStr, NULL); 
  set_colDef( colDefs, 1, "Description", 40, colors[1], drawit, charToStr, NULL); 
  set_colDef( colDefs, 2, "Players", 20, colors[1], drawit, intToStr, NULL); 
  set_colDef( colDefs, 3, "Ping", 10, colors[3], drawit, intToStr, sortit); 

  serverlist = new_wlist(10, 60,game->screen->vp_w-20, game->screen->vp_h-100,
			 16, 4, colDefs, 3, focus, action, mousefocus );
 
  newControl(trackerControls, (Wptr)serverlist, Wlistbox);

  //a button for testing
  newControl(trackerControls, (Wptr)new_wbutton(game->screen->vp_w/2-40, 40, 80, 15, "Refresh", NULL, NULL, NULL), WcontrolButton);

  //title
  x = game->screen->vp_w/2 - 1.5 * 7 *( game->screen->vp_w / (50 * 1.5) );
  y = game->screen->vp_h - 1.5 * h;
  newControl(trackerControls, (Wptr)new_wstatictext( x, y, x+1.5 * 7 *( game->screen->vp_w / (50 * 1.5)*14), h+5, "GLTRON SERVERS", h+3, gameFont, colors[0]), WstaticText);




  setCurrentControl( trackerControls, (Wptr)serverlist );

  current=-1;
}

void
mouseTracker(int buttons, int state, int x, int y)
{
  Wpoint pt;
  pt.v=y;
  pt.h=x;
  clickControls(trackerControls, buttons, state, pt);
}

void
cleanTracker()
{
}

void
mousemotionTracker( int mx, int my )
{
  Wpoint pt;

  pt.h=mx;
  pt.v=my;

  setMouse( mx, my );
  mouseControls( trackerControls, pt );
}

callbacks trackerscreenCallbacks = {
  displayTrackerScreen, idleTracker, keyTracker, initTracker,
  cleanTracker, NULL, mouseTracker, mousemotionTracker
};

