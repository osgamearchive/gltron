#include "video/video.h"
#include "game/game.h"
#include "filesystem/path.h"
#include "configuration/settings.h"

#include "Nebu_video.h"
#include "Nebu_input.h"
#include "Nebu_scripting.h"

void displayGame(void) {
  drawGame();
  nebu_System_SwapBuffers();
}

int initWindow(void) {
	int win_id;
	int flags;
	/* char buf[20]; */

	nebu_Video_SetWindowMode(0, 0, getSettingi("width"), getSettingi("height"));

	flags = SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_DEPTH | SYSTEM_STENCIL; 
	if(getSettingi("bitdepth_32"))
		flags |= SYSTEM_32_BIT;
	if(getSettingi("windowMode") == 0)
		flags |= SYSTEM_FULLSCREEN;

	nebu_Video_SetDisplayMode(flags);

	win_id = nebu_Video_Create("gltron");      

	if (win_id < 0) { 
		if( getSettingi("use_stencil") ) {
			flags &= ~SYSTEM_STENCIL;
			nebu_Video_SetDisplayMode(flags);
			win_id = nebu_Video_Create("gltron");      
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
		nebu_Input_Grab();
	} else {
		nebu_Input_Ungrab();
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
  nebu_Video_Destroy(d->win_id);
  // printf("[video] window destroyed\n");
}

void setupDisplay(Visual *d) {
  // fprintf(stderr, "[video] trying to create window\n");
  d->win_id = initWindow();
  // fprintf(stderr, "[video] window created\n");
  // initRenderer();
  // printRendererInfo();
  // printf("win_id is %d\n", d->win_id);
  // fprintf(stderr, "[status] loading art\n");
  loadArt();

  SystemReshapeFunc(reshape);
}

static void loadModels(void) {
	char *path;
	int i;
	/* load recognizer model */
	path = getPath(PATH_DATA, "recognizer.obj");
	if(path != NULL) {
		recognizer = gltron_Mesh_LoadFromFile(path, TRI_MESH);
		/* old code did normalize & invert normals & rescale to size = 60 */
	} else {
		fprintf(stderr, "fatal: could not load %s - exiting...\n", path);
		exit(1); /* OK: critical, installation corrupt */
	}
	free(path);
 
	/* load recognizer quad model (for recognizer outlines) */
	path = getPath(PATH_DATA, "recognizer_quad.obj");
	if(path != NULL) {
		recognizer_quad = gltron_Mesh_LoadFromFile(path, QUAD_MESH);
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
			lightcycle[i] = gltron_Mesh_LoadFromFile(path, TRI_MESH);
		} else {
			fprintf(stderr, "fatal: could not load model %s - exiting...\n", lc_lod_names[i]);
			exit(1); /* OK: critical, installation corrupt */
		}
	}
	free(path);
}

void initVideoData(void) {
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
		d->textures = (unsigned int*) malloc(TEX_COUNT * sizeof(unsigned int));
	}

	gPlayerVisuals = (PlayerVisual*) malloc(MAX_PLAYERS * sizeof(PlayerVisual));

	loadModels();

	for(i = 0; i < eHUDElementCount; i++)
	{
		gpHUD[i] = NULL;
	}
	changeDisplay(-1);
}

void initGameScreen(void) {
	Visual *d;
	d = gScreen;
	d->w = getSettingi("width");
	d->h = getSettingi("height"); 
	d->vp_x = 0; d->vp_y = 0;
	d->vp_w = d->w; d->vp_h = d->h;
}

void video_LoadLevel(void) {
	printf("[status] load/reload video data\n");

	if(gWorld)
		video_FreeLevel(gWorld);
	gWorld = video_CreateLevel();
	if(gWorld->scalable)
		video_ScaleLevel(gWorld, getSettingf("grid_size"));
}
	
void video_ResetData(void) {
	int i;

	printf("[status] reset video data\n");

	for(i = 0; i < game->players; i++) {
		PlayerVisual *pV = gPlayerVisuals + i;
		Player *p = game->player + i;
		{
			char name[32];
			sprintf(name, "model_diffuse_%d", i);
					scripting_GetGlobal(name, NULL);
			scripting_GetFloatArrayResult(pV->pColorDiffuse, 4);
			sprintf(name, "model_specular_%d", i);
					scripting_GetGlobal(name, NULL);
			scripting_GetFloatArrayResult(pV->pColorSpecular, 4);
			sprintf(name, "trail_diffuse_%d", i);
					scripting_GetGlobal(name, NULL);
			scripting_GetFloatArrayResult(pV->pColorAlpha, 4);
		}
		if(game->player[i].ai->active != AI_NONE) {
			p->data->impact_radius = 0.0;
			p->data->exp_radius = 0;
		} else {
			p->data->exp_radius = EXP_RADIUS_MAX;
		}
	}
}

void initDisplay(Visual *d, int type, int p, int onScreen) {
	float field_x = gScreen->vp_w / 32.0f;
	float field_y = gScreen->vp_h / 24.0f;
	d->h = gScreen->h;
	d->w = gScreen->w;
	d->vp_x = gScreen->vp_x + (int) ( vp_x[type][p] * field_x );
	d->vp_y = gScreen->vp_y + (int) ( vp_y[type][p] * field_y );
	d->vp_w = (int) ( vp_w[type][p] * field_x );
	d->vp_h = (int) ( vp_h[type][p] * field_y );
	d->onScreen = onScreen;
}  

static void defaultViewportPositions(void) {
	viewport_content[0] = 0;
	viewport_content[1] = 1;
	viewport_content[2] = 2;
	viewport_content[3] = 3;
}

/*
  autoConfigureDisplay - configure viewports so every human player has one
 */
static void autoConfigureDisplay(void) {
	int n_humans = 0;
	int i;
	int vp;

	defaultViewportPositions();

	/* loop thru players and find the humans */
	for (i=0; i < game->players; i++) {
		if (game->player[i].ai->active == AI_HUMAN) {
			viewport_content[n_humans] = i;
			n_humans++;
		}    
	}

	switch(n_humans) {
	case 0 :
		/*
			Not sure what the default should be for
			a game without human players. For now 
			just show a single viewport.
		*/
		/* fall thru */
	case 1 :
		vp = VP_SINGLE;
		break;
	case 2 :
		vp = VP_SPLIT;
		break;
	default :
		defaultViewportPositions();
		vp = VP_FOURWAY;
	}  
	updateDisplay(vp);
}

void changeDisplay(int view) {
	/* passing -1 to changeDisplay tells it to use the view from settings */
	if (view == -1) {
		view = getSettingi("display_type");
	}
	  
	if (view == 3) {
		autoConfigureDisplay(); 
	} else {
		defaultViewportPositions(); 
		updateDisplay(view);
	}

	// displayMessage(TO_STDOUT, "set display to %d", view);
	setSettingi("display_type", view);
}

void updateDisplay(int vpType) {
	int i;

	gViewportType = vpType;

	for (i = 0; i < game->players; i++) {
		gPlayerVisuals[i].display.onScreen = 0;
	}
	for (i = 0; i < vp_max[vpType]; i++) {
		initDisplay(& gPlayerVisuals[ viewport_content[i] ].display, 
			vpType, i, 1);
	}

}

void Video_Idle(void) { }
