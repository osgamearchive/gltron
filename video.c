#include "gltron.h"

void displayGame() {
  drawGame();
  SystemSwapBuffers();
}

int initWindow() {
  int win_id;
  int flags;
  unsigned char fullscreen = 0;
  /* char buf[20]; */

  SystemInitWindow(0, 0, getSettingi("width"), getSettingi("height"));

  if(getSettingi("windowMode") == 0) {
    fullscreen = SYSTEM_FULLSCREEN;
  }

  flags = SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_DEPTH;
  if(getSettingi("bitdepth_32"))
    flags |= SYSTEM_32_BIT;

  if(getSettingi("use_stencil"))
    flags |= SYSTEM_STENCIL;

  SystemInitDisplayMode(flags, fullscreen);

  win_id = SystemCreateWindow("gltron");      

  if (win_id < 0) { 
    if( getSettingi("use_stencil") ) {
      flags &= ~SYSTEM_STENCIL;
      SystemInitDisplayMode(flags, fullscreen);
      win_id = SystemCreateWindow("gltron");      
      if(win_id >= 0) {
	setSettingi("use_stencil", 0);
	goto SKIP;
      }
    }
    printf("[fatal] could not create window...exiting\n");
    exit(1); /* OK: critical, no visual */
  }

 SKIP:

  if(getSettingi("windowMode") == 0 || getSettingi("mouse_warp") == 1) {
    SystemGrabInput();
  } else {
    SystemUngrabInput();
  }

  return win_id;
}

void reshape(int x, int y) {
  if(x < getSettingi("height") || x < getSettingi("width"))
    initGameScreen();
  if(x > getSettingi("width") )
    gScreen->vp_x = (x - getSettingi("width")) / 2;
  if(y > getSettingi("height") )
    gScreen->vp_y = (y - getSettingi("height")) / 2;
  changeDisplay(-1);
}

void shutdownDisplay(Visual *d) {
  deleteTextures(d);
  deleteFonts();
  SystemDestroyWindow(d->win_id);
  // printf("[video] window destroyed\n");
}

void setupDisplay(Visual *d) {
  // fprintf(stderr, "[video] trying to create window\n");
  d->win_id = initWindow();
  // fprintf(stderr, "[video] window created\n");
  initRenderer();
  printRendererInfo();
  // printf("win_id is %d\n", d->win_id);
  // fprintf(stderr, "[status] loading art\n");
  loadArt();

  SystemReshapeFunc(reshape);
}

static void loadModels() {
	char *path;
	int i;
	/* load recognizer model */
	path = getPath(PATH_DATA, "recognizer.obj");
	if(path != NULL) {
		recognizer = readMeshFromFile(path, TRI_MESH);
		/* old code did normalize & invert normals & rescale to size = 60 */
	} else {
		fprintf(stderr, "fatal: could not load %s - exiting...\n", path);
		exit(1); /* OK: critical, installation corrupt */
	}
	free(path);
 
	/* load recognizer quad model (for recognizer outlines) */
	path = getPath(PATH_DATA, "recognizer_quad.obj");
	if(path != NULL) {
		recognizer_quad = readMeshFromFile(path, QUAD_MESH);
		/* old code did normalize & invert normals & rescale to size = 60 */
	} else {
		fprintf(stderr, "fatal: could not load %s - exiting...\n", path);
		exit(1); /* OK: critical, installation corrupt */
	}
	free(path);

	/* load lightcycle models */
	for(i = 0; i < LC_LOD; i++) {
		path = getPath(PATH_DATA, lc_lod_names[i]);
		if(path != NULL) {
			lightcycle[i] = readMeshFromFile(path, TRI_MESH);
		} else {
			fprintf(stderr, "fatal: could not load model %s - exiting...\n", lc_lod_names[i]);
			exit(1); /* OK: critical, installation corrupt */
		}
	}
	free(path);
}

void initVideoData() {
	int i;

  gScreen = (Visual*) malloc(sizeof(Visual));
  gViewportType = getSettingi("display_type"); 
  
	{
		Visual *d = gScreen;
		d->w = getSettingi("width"); 
		d->h = getSettingi("height"); 
		d->vp_x = 0; d->vp_y = 0;
		d->vp_w = d->w; d->vp_h = d->h;
		d->onScreen = -1;
		d->textures = (unsigned int*) malloc(game_textures * sizeof(unsigned int));
	}

	gPlayerVisuals = (PlayerVisual*) malloc(MAX_PLAYERS * sizeof(PlayerVisual));

  for(i = 0; i < game->players; i++) {
		PlayerVisual *pV = gPlayerVisuals + i;

    pV->display = (Visual*) malloc(sizeof(Visual));
    pV->camera = (Camera*) malloc(sizeof(Camera));
    pV->camera->type = (CameraType*) malloc(sizeof(CameraType));
  }

	loadModels();

  changeDisplay(-1);
}

void initGameScreen() {
  Visual *d;
  d = gScreen;
  d->w = getSettingi("width");
  d->h = getSettingi("height"); 
  d->vp_x = 0; d->vp_y = 0;
  d->vp_w = d->w; d->vp_h = d->h;
}
