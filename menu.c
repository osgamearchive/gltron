#include "gltron.h"
#include <string.h>

Menu *pCurrent = 0;

#define MENU_BUFSIZE 100


// static char *speed_list[] = {  "boring", "normal", "fast", "crazy", "custom", NULL };
static char *speed_list[] = {  "boring", "normal", "fast", "crazy", NULL };
static char *player_list[] = { "human", "computer", "none", NULL };
static char *arena_list[] = { "tiny", "medium", "big", "vast", "extreme", NULL };
static char *lod_list[] = { "normal", "lower", "ugly", NULL };
static char *shadow_lod_list[] = { "high", "normal", "low", NULL };
static char *ai_list[] = { "dumb", "normal", "strong", "the MCP himself", NULL };
static char *filter_list[] = { "bilinear", "trilinear", NULL };
static char *camera_list[] = { "circling", "behind", "cockpit", "mouse", NULL };
static char *viewports_list[] = { "single", "split", "4 player", NULL };

static char **clists[] = { speed_list, player_list, arena_list, lod_list,
			   ai_list, filter_list, camera_list, viewports_list,
			   shadow_lod_list,
			   NULL 
};

void changeAction(char *name) {
#ifdef SOUND
  if(strstr(name, "playMusic") == name) {
    if(game->settings->playMusic == 0)
      stopSound();
    else if(game->settings->soundIndex >= 0)
      playSound();
  } else if(strstr(name, "musicVolume") == name) {
    setMusicVolume(game->settings->musicVolume);
  } else if(strstr(name, "fxVolume") == name) {
    setFxVolume(game->settings->fxVolume);
    playMenuFX(fx_highlight);
  } else if(strstr(name, "song") == name) {
    if(game->settings->soundIndex != -1) {
      char *tmp;
      char *path;
      list *p;
      int c;

      c = 0;
      for(p = game->settings->soundList; p->next != NULL; p = p->next) {
	if(c == game->settings->soundIndex)
	  break;
	c++;
      }      
      if(c == game->settings->soundIndex) {
	path = getFullPath(MUSIC_DIR);
	tmp = malloc(strlen(path) + 1 + /* seperator */
		     strlen((char*) p->data) + 1);
	sprintf(tmp, "%s%c%s", path, SEPERATOR, 
		(char*) p->data);
	fprintf(stderr, "loading song %s\n", tmp);

	if(game->settings->playMusic)
	  stopSound();
	loadSound(tmp);
	free(tmp);
	if(game->settings->playMusic)
	  playSound();
      }
    } else {
      if(game->settings->playMusic)
	stopSound();
    }
  }
#endif
  if(strstr(name, "use_mipmaps") == name ||
     strstr(name, "mipmap_filter") == name) {
    deleteFonts();
    deleteTextures(game->screen);
    initFonts();
    initTexture(game->screen);
  }
  else if(strstr(name, "game_speed") == name) {
    game->settings->current_speed = 
      default_speeds[ game->settings->game_speed ];
    initData();
  } 
  else if(strstr(name, "arena_size") == name) {
    game->settings->grid_size = 
      default_arena_sizes[ game->settings->arena_size ];
    initData();
  } 
  else if(strstr(name, "display_type") == name) {
    changeDisplay();
  } 
  else if(strstr(name, "resetScores") == name)
    resetScores();

  else if(strstr(name, "ai_player") == name) {
    int c;
    int *v;

    /* printf("changing AI status\n"); */
    c = name[9] - '0';
    v = getVi(name);
    game->player[c - 1].ai->active = *v;
    /* printf("changed AI status for player %c\n", c + '0'); */
  } 
  else if(strstr(name, "windowMode") == name) {
    initGameScreen();
    shutdownDisplay(game->screen);
    setupDisplay(game->screen);

    updateCallbacks();
    changeDisplay();
  } else if(strstr(name, "artpack") == name) {
    reloadArt();
  } else if(strstr(name, "mouse_warp") == name) {
    if(game->settings->mouse_warp == 1)
      SystemGrabInput();
    else
      if(game->settings->windowMode == 1) SystemUngrabInput();
  } else if(strstr(name, "camType") == name) {
    int i;
    for(i = 0; i < game->players; i++)
      if(game->player[i].ai->active == 0)
	initCamera(game->player[i].camera, 
		   game->player[i].data, 
		   game->settings->camType);
  }
}

void menuAction(Menu *activated, int type) {
  int x, y;
  char c;
  int *piValue;
  if(activated->nEntries > 0) {
    pCurrent = activated;
    pCurrent->iHighlight = 0;
  } else if (type == MENU_ACTION) {
    switch(activated->szName[1]) { /* second char */
    case 'q': 
      saveSettings(); 
      /* SystemExit(); */
      switchCallbacks(&creditsCallbacks);
      break;
    case 'r': 
      initData();
      game2->mode = GAME_SINGLE;
      switchCallbacks(&pauseCallbacks);
      break;
#ifdef __NETWORK__
    case 'n':
      switch( activated->szName[3] )
	{
	case 't':
	  printf("connect menu chosen\n");
	  isConnected=0;
	  isLogged=0;
	  switchCallbacks(&netConnectCallbacks);
	  break;
	case 's':
	  switchCallbacks(&serverCallbacks);
	  break;
	case 'p':
	  switchCallbacks(&portCallbacks);
	  break;
	case 'n':
	  switchCallbacks(&nicknameCallbacks);
	  break;
	case 'o':
	  if( ! tracker_connect() )
	    switchCallbacks(&trackerscreenCallbacks);
	  break;
	case 'l':
	  switchCallbacks(&trackerCallbacks);
	  break;
	case 'c':
	  switchCallbacks(&tportCallbacks);
	  break;
	}
      break;	
#endif
    case 'a': // this menu code is turning uglier and uglier 
      printf("artpack menu chosen\n");
      artpack_index++;
      if(artpack_list[artpack_index] == NULL)
	artpack_index = 0;
      initMenuCaption(activated);
      changeAction(activated->szName + 3);
      break;
#ifdef __NETWORK__
    case 'd':
      if(activated->szName[2] == 'e') { /* load default settings */
	initDefaultSettings();
	initMenuCaptions();
	reloadArt();
	fprintf(stderr, "loaded default settings\n");
	break;
      } else if(activated->szName[2] == 'p') {
	initData();
	if(startPlaying("demo.txt")) {
	  fprintf(stderr, "starting \"play demo\" failed\n");
	  break;
	}
	if(readDemoInfo()) {
	  fprintf(stderr, "reading demo information failed\n");
	  break;
	}
	applyGameInfo();
	game2->mode = GAME_PLAY;
	switchCallbacks(&pauseCallbacks);
      } else if(activated->szName[2] == 'r') {
	initData();
	if(startRecording("demo.txt")) {
	  fprintf(stderr, "starting \"record demo\" failed\n");
	  break;
	}
	if(writeDemoInfo()) {
	  fprintf(stderr, "writing demo information failed\n");
	  break;
	}
	game2->mode = GAME_SINGLE_RECORD;
	switchCallbacks(&pauseCallbacks);
      }
      break;
#endif
    case 'v':
      sscanf(activated->szName, "%cv%dx%d ", &c, &x, &y);
      game->settings->width = x;
      game->settings->height = y;

      initGameScreen();
      shutdownDisplay(game->screen);
      setupDisplay(game->screen);

      updateCallbacks();
      changeDisplay();
      break;
    case 'k': {
      int player, turn;
      int i;
      /* TODO: set target key name */
      sscanf(activated->szName + 1, "k_%d_%d ", &player, &turn);
      for(i = 0; i < KEY_ACTIONS_N; i++)
	if(key_actions[i].player == player &&
	   key_actions[i].turn == turn) {
	  configureKeyEntry = &(key_actions[i].key);
	  break;
	}
      configureKeyMenu = activated;
      switchCallbacks(&configureCallbacks);
      break;
    }
    case 't':
      switch(activated->szName[2]) {
      case 'i':
	piValue = getVi(activated->szName + 4);
	if(piValue != NULL) {
	  *piValue = (*piValue - 1) * (-1);
	  initMenuCaption(activated);
	  changeAction(activated->szName + 4);
	}
	break;
      case 'l':
	{
	  char buf[64];
	  // int max_value;
	  int dummy;
	  int clist_index;
	  
	  // sscanf(activated->szName, "stl_%d_%d_%s", &dummy, &max_value, buf);
	  sscanf(activated->szName, "stl_%d_%d_%s", &clist_index, &dummy, buf);
	  piValue = getVi(buf);
	  if(piValue != NULL) {
	    (*piValue)++;
	    // if(*piValue > max_value) *piValue = 0;
	    if(clists[clist_index][*piValue] == NULL) *piValue = 0;
	    initMenuCaption(activated);
	    changeAction(buf);
	    break;
	  }
	}
      }
      break;
    case 'p':
      changeAction(activated->szName + 4);
    case 'c':
      chooseCallback(activated->szName + 3);
      break;
    case 'm':
      changeAction(activated->szName + 3);
      break;
    default: printf("got action for menu %s\n", activated->szName); break;
    }
  } else if(type == MENU_LEFT || type == MENU_RIGHT) {
    switch(activated->szName[1]) {
    case 's': 
      {
	float *pfValue;
	float min, max, step;
	char buf[64];
	sscanf(activated->szName, "ssf_%f_%f_%f_%s", &min, &max, &step, buf);
	pfValue = getVf(buf);
	if(pfValue != NULL) {
	  *pfValue = (type == MENU_LEFT) ? (*pfValue - step) : (*pfValue + step);
	  if(*pfValue < min) *pfValue = min;
	  if(*pfValue > max) *pfValue = max;
	}
	initMenuCaption(activated);
	changeAction(buf);
	break;
      }
    case 'm':
      switch(type) {
      case MENU_LEFT:
	game->settings->soundIndex--;
	if(game->settings->soundIndex < -1) game->settings->soundIndex = -1;
	break;
      case MENU_RIGHT:
	game->settings->soundIndex++;
	if(game->settings->soundIndex >= game->settings->soundSongCount) 
	  game->settings->soundIndex = game->settings->soundSongCount - 1;
	break;
      }
      initMenuCaption(activated);
      break;
    }
  }
}

void initMenuCaption(Menu *m) {
  int *piValue;

  /* TODO support all kinds of types */
  switch(m->szName[0]) {
  case 's':
    switch(m->szName[1]) {
    case 'a':
      printf("setting artpack menu caption\n");
      sprintf(m->display.szCaption, m->szCapFormat, 
	      artpack_list[artpack_index]);
      break;
#ifdef __NETWORK__
    case 'n':
      switch( m->szName[3] )
	{
	case 'n':
	  printf("setting network menu caption ( nickname )\n");
	  sprintf(m->display.szCaption, m->szCapFormat, 
		  game->settings->nickname);
	  break;
	case 'p':
	  printf("setting network menu caption ( port )\n");
	  sprintf(m->display.szCaption, m->szCapFormat, 
		  game->settings->port);
	  break;
	case 's':
	  printf("setting network menu caption ( server )\n");
	  sprintf(m->display.szCaption, m->szCapFormat, 
		  game->settings->server);
	  break;
	case 'l':
	  sprintf(m->display.szCaption, m->szCapFormat, 
		  game->settings->tracker);
	  break;
	case 'c':
	  sprintf(m->display.szCaption, m->szCapFormat, 
		  game->settings->tport);
	  break;
	}
      break;
#endif
    case 't':
      switch(m->szName[2]) {
      case 'i':
	/* printf("dealing with %s\n", m->szName); */
	piValue = getVi(m->szName + 4);
	if(piValue != NULL) {
	  if(*piValue == 0) sprintf(m->display.szCaption,
				    m->szCapFormat, "off");
	  else sprintf(m->display.szCaption, m->szCapFormat, "on");
	  /* printf("changed caption to %s\n", m->display.szCaption); */
	} /* else printf("can't find value for %s\n", m->szName + 4); */
	break;
      case 'l':
	{
	  int clist_index;
	  int dummy;
	  char buf[64];
	  sscanf(m->szName, "stl_%d_%d_%s ", &clist_index, &dummy, buf);
	  piValue = getVi(buf);
	  /* TODO: bounds checking */
	  sprintf(m->display.szCaption, m->szCapFormat,
		  (clists[clist_index])[*piValue]);
	  break;
	}
      } /* end szName[2] */
      break;
    case 'k': 
      { /* set key name in menu */
      int player, turn;
      int i;
      int key;
      char *caption = NULL;
      sscanf(m->szName + 1, "k_%d_%d ", &player, &turn);
      for(i = 0; i < KEY_ACTIONS_N; i++)
	if(key_actions[i].player == player &&
	   key_actions[i].turn == turn) {
	  key = key_actions[i].key;
	  caption = SystemGetKeyName(key);
	  break;
	}
      sprintf(m->display.szCaption, m->szCapFormat, caption);
      /* free(caption); */
      break;
      }
    case 's': /* slider */
      switch(m->szName[2]) {
	case 'f':
	  {
	    char buf[64];
	    float *pfValue;
	    sscanf(m->szName, "ssf_%*f_%*f_%*f_%s ", buf);
	    pfValue = getVf(buf);
	    if(pfValue != NULL) {
	      sprintf(m->display.szCaption, m->szCapFormat, *pfValue * 100);
	    }
	    break;
	  }
      }
      break;
    case 'm': /* song */
      {
	list *p;
	int c = 0;
	for(p = game->settings->soundList; p->next != NULL; p = p->next) {
	  if(c == game->settings->soundIndex)
	    sprintf(m->display.szCaption, m->szCapFormat, 
		    (char*) p->data + strlen(SONG_PREFIX));
	  c++;
	}
	if(game->settings->soundIndex == -1)
	  sprintf(m->display.szCaption, m->szCapFormat, "none");
      }
      break;
    }
    break;
  default:
    sprintf(m->display.szCaption, "%s", m->szCapFormat);
    /* printf("using default capformat\n"); */
  }
  /* printf("[menu] cap-format: %s, caption: %s\n", m->szCapFormat,
     m->display.szCaption); */

}

void getNextLine(char *buf, int bufsize, FILE* f) {
  fgets(buf, bufsize, f);
  while((buf[0] == '\n' || buf[0] == '#') &&
	fgets(buf, bufsize, f));
}

/* loadMenu returns the current Menu (sub-) tree */

Menu* loadMenu(FILE* f, char* buf, Menu* parent, int level) {
  Menu* m;
  int i;


  if(level > 4) {
    printf("recursing level > 4 - aborting\n");
    exit(1);
  }

  m = (Menu*) malloc(sizeof(Menu));
  m->parent = parent;
  getNextLine(buf, MENU_BUFSIZE, f);
  sscanf(buf, "%d ", &(m->nEntries));

  getNextLine(buf, MENU_BUFSIZE, f);
  /* TODO: this is horribly broken, 31 is probably wrong
     anyway, so handle arbitrary strings or use magic enums */
  buf[31] = 0; /* enforce menu name limit; */
  sprintf(m->szName, "%s", buf);
  if(*(m->szName + strlen(m->szName) - 1) == '\n')
    *(m->szName + strlen(m->szName) - 1) = 0;
  
  getNextLine(buf, MENU_BUFSIZE, f);
  buf[31] = 0; /* enforce menu caption limit; */
  sprintf(m->szCapFormat, "%s", buf);
  /* remove newline */
  for(i = 0; *(m->szCapFormat + i) != 0; i++)
    if (*(m->szCapFormat + i) == '\n') {
      *(m->szCapFormat + i) = 0;
      break;
    }

  initMenuCaption(m);
	
  /* printf("menu '%s': %d entries\n", m->szName, m->nEntries); */
  if(m->nEntries > 0) { /* contains a submenu */
    m->pEntries = malloc(sizeof(Menu*) * m->nEntries);
    for(i = 0; i < m->nEntries; i++) { /* load these sub menus */
      /* printf("loading menu number %d\n", i); */
      if(i > 10) {
	printf("item limit reached - aborting\n");
	exit(1);
      }
      *(m->pEntries + i) = loadMenu(f, buf, m, level + 1);
    }
  }

  return m;
}

void initMenuCaptions() {
  node *head;
  node *t;
  node *z;
  int j;
  Menu *m;

  z = (node*) malloc(sizeof(node));
  z->next = z;
  head = (node*) malloc(sizeof(node));
  head->next = z;
  
  t = (node*) malloc(sizeof(node));
  t->data = *(pMenuList);
  t->next = head->next;
  head->next = t;

  while(head->next != z) {
    t = head->next;
    head->next = t->next;
    m = (Menu*) t->data;
    free(t);
    /* visit m */

    initMenuCaption(m);

    /* push all of m's submenus */
    for(j = 0; j < m->nEntries; j++) {
      t = (node*) malloc(sizeof(node));
      t->data = *(m->pEntries + j);
      t->next = head->next;
      head->next = t;
    }
  }
}

Menu** loadMenuFile(char *filename) {
  char buf[MENU_BUFSIZE];
  FILE* f;
  Menu* m;
  Menu** lst = NULL;
  int nMenus;
  int i, j;
  node *head;
  node *t;
  node *z;
  int sp = 0;

  if((f = fopen(filename, "r")) == NULL)
    return NULL;
  /* read count of Menus */
  getNextLine(buf, MENU_BUFSIZE, f);
  sscanf(buf, "%d ", &nMenus);
  if(nMenus <= 0) return NULL;

  /* allocate space for data structures */
  lst = (Menu**) malloc(sizeof(Menu*) * nMenus);

  /* load data */
  for(i = 0; i < nMenus; i++) {
    /* printf("loading menu set %d\n", i); */
    /* TODO: bad magic */
    if(i > 10) exit(1);
    *(lst + i) = loadMenu(f, buf, NULL, 0);
  }
  fclose(f);

  /* TODO(3): now since I eliminated the need for cx/cy, why */
  /* do I need to traverse the Menu Tree? Just to set the colors??? */

  /* traverse Menu Tree and set Menu Color to some boring default */
  /* printf("finished parsing file - now traversing menus\n"); */
  /* setup stack */

  z = (node*) malloc(sizeof(node));
  z->next = z;
  head = (node*) malloc(sizeof(node));
  head->next = z;
  
  for(i = 0; i < nMenus; i++) {
    t = (node*) malloc(sizeof(node));
    t->data = *(lst + i);
    t->next = head->next;
    head->next = t;
    sp++;
    while(head->next != z) {
      t = head->next;
      head->next = t->next;
      m = (Menu*) t->data;
      free(t);
      /* printf("stack count: %d\n", --sp); */
      /* printf("visiting %s\n", m->szName); */
      /* visit m */

      /* copy default colors into menu entry */

      memcpy(m->display.fgColor, menu_fgColor, sizeof(menu_fgColor) );
      memcpy(m->display.hlColor1, menu_hlColor1, sizeof(menu_hlColor1) );
      memcpy(m->display.hlColor2, menu_hlColor2, sizeof(menu_hlColor2) );

      /* push all of m's submenus */
      for(j = 0; j < m->nEntries; j++) {
	t = (node*) malloc(sizeof(node));
	t->data = *(m->pEntries + j);
	t->next = head->next;
	head->next = t;
	/* printf("pushing %s\n", ((Menu*)t->data)->szName); */
	/* printf("stack count: %d\n", ++sp); */
	
      }
    }
  }
  return lst;
}

void drawMenu(gDisplay *d) {
  /* draw Menu pCurrent */

  int i;
  int x, y, size, lineheight;
  int hsize, vsize;
  int maxw = 0;

  rasonly(d);

#define MENU_TEXT_START_X 0.08
#define MENU_TEXT_START_Y 0.40

#define MENU_WIDTH 0.80
#define MENU_HEIGHT 0.40

#define MENU_TEXT_LINEHEIGHT 1.5

  x = (int) (d->vp_w * MENU_TEXT_START_X);
  y = (int) (d->vp_h * MENU_TEXT_START_Y);

  /* new stuff: calculate menu dimensions */
  for(i = 0; i < pCurrent->nEntries; i++) {
    int len;
    len =  strlen(((Menu*)*(pCurrent->pEntries + i))->display.szCaption);
    if(len > maxw)
      maxw = len;
  }

  /* adjust size so menu fits into MENU_WIDTH/HEIGHT */

  hsize = (int) ((float)d->vp_w * MENU_WIDTH / (float)maxw );
  vsize = (int) ((float)d->vp_h * MENU_HEIGHT / 
		 ( (float)pCurrent->nEntries * MENU_TEXT_LINEHEIGHT));

  size = (hsize < vsize) ? hsize : vsize;

  lineheight = (int)( (float) size * MENU_TEXT_LINEHEIGHT);  

  /* printf("%d %d %d %d %d\n", x, y, size, maxw, pCurrent->nEntries); */
  /* draw the entries */

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for(i = 0; i < pCurrent->nEntries; i++) {
    if(i == pCurrent->iHighlight) {
      // glColor4fv(pCurrent->display.hlColor);
      float color[4];
      int j;
      float t;
      int time = (SystemGetElapsedTime() - menutime) & 4095; 
      t = sin( time * M_PI / 2048.0 ) / 2.0 + 0.5;
      for(j = 0; j < 4; j++) {
	color[j] = 
	  t * pCurrent->display.hlColor1[j] + 
	  (1 - t) * pCurrent->display.hlColor2[j];
      }
      glColor4fv(color);
    } else 
      glColor4fv(pCurrent->display.fgColor);

    drawText(guiFtx, x, y, size,
	     ((Menu*)*(pCurrent->pEntries + i))->display.szCaption);

    if(i == pCurrent->iHighlight) 
      drawSoftwareHighlight(x, y, size, ((Menu*)*(pCurrent->pEntries + i))->display.szCaption);

    y -= lineheight;
  }
  
  glDisable(GL_BLEND);
}
