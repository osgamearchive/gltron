#include "video/video.h"
#include "game/game.h"

#include "video/skybox.h"
#include "video/recognizer.h"
#include "video/explosion.h"

#include "base/nebu_math.h"

// static float arena[] = { 1.0, 1.2, 1, 0.0 };

#define MAX_LOD_LEVEL 3
static int lod_dist[MAX_LOD_LEVEL + 1][LC_LOD + 1] = { 
  { 1000, 1000, 1000 }, /* insane */
  { 100, 200, 400 }, /* high */
  { 30, 100, 200 }, /* low */
  { 10, 30, 150 } /* ugly */
};

/* spoke colors */
static float SpokeColor[4] = {1.0, 1.0, 1.0, 1.0};
static float NoSpokeColor[4] = {0.0, 0.0, 0.0, 1.0};

void clearScreen() {
  glClearColor(gSettingsCache.clear_color[0], 
               gSettingsCache.clear_color[1], 
               gSettingsCache.clear_color[2],
               0);

  if(gSettingsCache.use_stencil) {
    glClearStencil(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  } else {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }
}

void drawGame(void) {
	GLint i;

	polycount = 0;

	clearScreen();

	glShadeModel( GL_SMOOTH );
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	for(i = 0; i < vp_max[gViewportType]; i++) {
		Player *p = game->player + viewport_content[i];
		PlayerVisual *pV = gPlayerVisuals + viewport_content[i];
		Visual *d = & pV->display;

		if(d->onScreen == 1) {
			glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
			drawCam(p, pV);

			/* hud stuff for every player */
			drawHUD(p, pV);
		}
	}
	/* printf("%d polys\n", polycount); */
}

/* 
float GetDistance(float *v, float *p, float *d) {
  float diff[3];
  float tmp[3];
  float t;
  vsub(v, p, diff);
  t = scalarprod(d, diff) / scalarprod(d, d);
  vcopy(d, tmp);
  vmul(tmp, t);
  vsub(diff, tmp, tmp);
  return sqrtf( scalarprod(tmp, tmp) );
}
*/

static float dirangles[] = { 0, -90, -180, 90, 180, -270 };

float getDirAngle(int time, Player *p) {
  int last_dir;
  float dirAngle;

  if(time < TURN_LENGTH) {
    last_dir = p->data->last_dir;
    if(p->data->dir == 3 && last_dir == 2)
      last_dir = 4;
    if(p->data->dir == 2 && last_dir == 3)
      last_dir = 5;
    dirAngle = ((TURN_LENGTH - time) * dirangles[last_dir] +
		time * dirangles[p->data->dir]) / TURN_LENGTH;
  } else
    dirAngle = dirangles[p->data->dir];

  return dirAngle;
}

void doCycleTurnRotation(PlayerVisual *pV, Player *p) {
  int neigung_dir = -1;
	int time = game2->time.current - p->data->turn_time;
  float dirAngle = getDirAngle(time, p);

  glRotatef(dirAngle, 0, 0, 1);

#define neigung 25
  if(time < TURN_LENGTH && p->data->last_dir != p->data->dir) {
    float axis = 1.0f;
    if(p->data->dir < p->data->last_dir && p->data->last_dir != 3)
      axis = -1.0;
    else if((p->data->last_dir == 3 && p->data->dir == 2) ||
	    (p->data->last_dir == 0 && p->data->dir == 3))
      axis = -1.0;
    glRotated(neigung * sin(PI * time / TURN_LENGTH),
	      0.0, axis * neigung_dir, 0.0);
  }
#undef neigung
}

void drawCycleShadow(PlayerVisual *pV, Player *p, int lod, int drawTurn) {
  gltron_Mesh *cycle;
  int turn_time = game2->time.current - p->data->turn_time;
      
  if(turn_time < TURN_LENGTH && !drawTurn)
    return;

  if(pV->exp_radius != 0)
    return;

  cycle = lightcycle[lod];

  /* states */

  glEnable(GL_CULL_FACE);

  /* transformations */

  glPushMatrix();
	{
		float x, y;
		getPositionFromData(&x, &y, p->data);
		glTranslatef(x,y, 0.0);
	}
  glMultMatrixf(shadow_matrix);
  if (gSettingsCache.turn_cycle) {
    doCycleTurnRotation(pV, p);
  } else if (pV->exp_radius == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }
  glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

  /* render */

  gltron_Mesh_Draw(cycle, TRI_MESH);

  /* restore */
  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

void drawCycle(Player *p, PlayerVisual *pV, int lod, int drawTurn) {
	gltron_Mesh *cycle = lightcycle[lod];
	float x, y;

	unsigned int spoke_time = game2->time.current - pV->spoke_time;
	int turn_time = game2->time.current - p->data->turn_time;

	if(turn_time < TURN_LENGTH && !drawTurn)
		return;

	getPositionFromData(&x, &y, p->data);
	glPushMatrix();
	glTranslatef(x, y, 0.0);

	if (pV->exp_radius == 0 && gSettingsCache.turn_cycle == 0) {
		glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
	}

	if (gSettingsCache.turn_cycle) { 
		doCycleTurnRotation(pV, p);
	}

	if(p->data->wall_buster_enabled) {
		float black[] = { 0, 0, 0, 1};
		float white[] = { 1, 1, 1, 1};
		gltron_Mesh_SetMaterialColor(cycle, "Hull", eDiffuse, black);
		// gltron_Mesh_SetMaterialColor(cycle, "Hull", eAmbient, black);
		gltron_Mesh_SetMaterialColor(cycle, "Hull", eSpecular, white); 
	} else {
		gltron_Mesh_SetMaterialColor(cycle, "Hull", eDiffuse, pV->pColorDiffuse); 
		gltron_Mesh_SetMaterialColor(cycle, "Hull", eSpecular, pV->pColorSpecular); 
	}

	if (pV->exp_radius == 0) {
		glEnable(GL_NORMALIZE);

		glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

		/* draw spoke animation */
		if (spoke_time > 140 - (p->data->speed * 10) 
			&& game->pauseflag == PAUSE_GAME_RUNNING) {
			if (pV->spoke_state == 1) {
			pV->spoke_state = 0;
			gltron_Mesh_SetMaterialColor(cycle, "Spoke", eSpecular, SpokeColor);
			gltron_Mesh_SetMaterialColor(cycle, "Spoke", eAmbient, SpokeColor);
			} else {
			pV->spoke_state = 1;
			gltron_Mesh_SetMaterialColor(cycle, "Spoke", eSpecular, NoSpokeColor);
			gltron_Mesh_SetMaterialColor(cycle, "Spoke", eAmbient, NoSpokeColor);
			}
			pV->spoke_time = game2->time.current;
		}

		if (gSettingsCache.light_cycles) {
			glEnable(GL_LIGHTING); // enable OpenGL lighting for lightcycles
		}
		glEnable(GL_CULL_FACE);
		gltron_Mesh_Draw(cycle, TRI_MESH);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
	} else if(pV->exp_radius < EXP_RADIUS_MAX) {
		glEnable(GL_BLEND);

		if (gSettingsCache.show_impact) {
			drawImpact(pV);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

		if (gSettingsCache.light_cycles) {
			glEnable(GL_LIGHTING); // enable OpenGL lighting for lightcycles
		}
		gltron_Mesh_DrawExplosion(cycle, pV->exp_radius);
		glDisable(GL_LIGHTING); // disable ligthing after lightcycles
		glDisable(GL_BLEND);
	}
	glPopMatrix();
}
 
int playerVisible(Player *eye, Player *target) {
  vec3 v1, v2, tmp;
	
  float s;
  float d;
  int i;
  int lod_level;
	float x, y;

  vec3_Sub(&v1, (vec3*) eye->camera->target, (vec3*) eye->camera->cam);
	vec3_Normalize(&v1, &v1);
	
	getPositionFromData(&x, &y, target->data);
	tmp.v[0] = x;
  tmp.v[1] = y;
  tmp.v[2] = 0;
	
  lod_level = (gSettingsCache.lod > MAX_LOD_LEVEL) ? 
    MAX_LOD_LEVEL : gSettingsCache.lod;

  /* calculate lod */
  vec3_Sub(&v2, (vec3*) eye->camera->cam, &tmp);
  d = vec3_Length(&v2);
  for(i = 0; i < LC_LOD && d >= lod_dist[lod_level][i]; i++);
  if(i >= LC_LOD)
    return -1;

  vec3_Sub(&v2, &tmp, (vec3*) eye->camera->cam);
  vec3_Normalize(&v2, &v2);
  s = vec3_Dot(&v1, &v2);
  /* maybe that's not exactly correct, but I didn't notice anything */
  d = cosf((gSettingsCache.fov / 2) * 2 * PI / 360.0);
  /*
    printf("v1: %.2f %.2f %.2f\nv2: %.2f %.2f %.2f\ns: %.2f d: %.2f\n\n",
    v1[0], v1[1], v1[2], v2[0], v2[1], v2[2],
    s, d);
  */
  if(s < d-(lightcycle[i]->BBox.fRadius*2))
    return -1;
  else
    return i;
}

void drawPlayers(Player *p, PlayerVisual *pV) {
  int i;

  for(i = 0; i < game->players; i++) {
		int lod;
		int drawTurn = 1;

		if (gSettingsCache.camType == CAM_TYPE_COCKPIT && 
				p == &game->player[i])
			drawTurn = 0;

		lod = playerVisible(p, &(game->player[i]));
		if (lod >= 0) { 
			drawCycle(game->player + i, gPlayerVisuals + i, lod, drawTurn);
		}
	}
}

void drawPlanarShadows(Player *p) {
	int i;

	if(gSettingsCache.use_stencil) {
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glStencilFunc(GL_EQUAL, 1, 255);
		glEnable(GL_BLEND);
		glColor4fv(gCurrentShadowColor);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glColor3f(0, 0, 0);
		glDisable(GL_BLEND);
	}

	/* shadows on the floor: cycle, recognizer, trails */
	if (gSettingsCache.show_recognizer) {
		drawRecognizerShadow();
	}

	for(i = 0; i < game->players; i++) {
	int lod = playerVisible(p, game->player + i);
		if (lod >= 0) {
			int drawTurn = 1;
			if (!gSettingsCache.camType == CAM_TYPE_COCKPIT ||
	 			p != &game->player[i])
			{
				drawTurn = 0;
			}
			drawCycleShadow(gPlayerVisuals + i, game->player + i, lod, drawTurn);
		}
		if (game->player[i].data->trail_height > 0 )
			drawTrailShadow(game->player + i, gPlayerVisuals + i);
	}

	if(gSettingsCache.use_stencil)
		glDisable(GL_STENCIL_TEST);

	glDisable(GL_BLEND);
}

void drawWorld(Player *p, PlayerVisual *pV) {
	int i;

	nebu_Video_CheckErrors("before world");

	setupLights(eWorld);

	if (gSettingsCache.show_recognizer &&
		p->data->speed != SPEED_GONE) {
		drawRecognizer();
	}

	if (gSettingsCache.show_wall == 1) {
		glColor3f(1,1,1);
		drawWalls();
	}

	setupLights(eCycles);

	drawPlayers(p, pV);

	setupLights(eWorld);

	{
		TrailMesh mesh;
		mesh.pVertices = (vec3*) malloc(1000 * sizeof(vec3));
		mesh.pNormals = (vec3*) malloc(1000 * sizeof(vec3));
		mesh.pColors = (unsigned char*) malloc(1000 * 4 * sizeof(float));
		mesh.pTexCoords = (vec2*) malloc(1000 * sizeof(vec2));
		mesh.pIndices = (unsigned short*) malloc(1000 * 2);

		for(i = 0; i < game->players; i++) {
			if (game->player[i].data->trail_height > 0 ) {
				int vOffset = 0;
				int iOffset = 0;
				mesh.iUsed = 0;
				trailGeometry(game->player + i, gPlayerVisuals + i,
					&mesh, &vOffset, &iOffset);
				bowGeometry(game->player + i, gPlayerVisuals + i,
					&mesh, &vOffset, &iOffset);
				trailStatesNormal(game->player + i, gScreen->textures[TEX_DECAL]);
				trailRender(&mesh);
				trailStatesRestore();
			}
		}
		free(mesh.pVertices);
		free(mesh.pNormals);
		free(mesh.pColors);
		free(mesh.pTexCoords);
		free(mesh.pIndices);
	}

	for(i = 0; i < game->players; i++)
		if (game->player[i].data->trail_height > 0 )
			drawTrailLines(game->player + i, gPlayerVisuals + i);

	nebu_Video_CheckErrors("after world");
}

static float getReflectivity() {
	float reflectivity = getSettingf("reflection");
	if(reflectivity < 0)
		reflectivity = getVideoSettingf("reflection");

	// need stencil for reflections
	if(gSettingsCache.use_stencil == 0)
		reflectivity = 0;
	return reflectivity;
}

void drawCam(Player *p, PlayerVisual* pV) {
	int i;
	float up[3] = { 0, 0, 1 };
	Visual *d = & pV->display;
	
	float reflectivity = getReflectivity();
	// compute shadow color based on glocal constant & reflectivity
	for(i = 0; i < 4; i++) 
		gCurrentShadowColor[i] = gShadowColor[i] * (1 - reflectivity);

	glColor3f(0.0, 1.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	doPerspective(gSettingsCache.fov, (float) d->vp_w / (float) d->vp_h,
		gSettingsCache.znear, box2_Diameter(& game2->level->boundingBox) * 6.5f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	doLookAt(p->camera->cam, p->camera->target, up);
	glDisable(GL_LIGHTING); // initial config at frame start
	glDisable(GL_BLEND); // initial config at frame start

	// disable writes to alpha
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

	/* skybox */
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	drawSkybox( box2_Diameter( & game2->level->boundingBox ) * 2.5f );

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	/* skybox done */

	/* floor */ 
	if(reflectivity == 0) {
		// draw floor to fb and stencil (set to 1),
		// using alpha-blending
		// TODO: draw floor alpha to fb
		video_Shader_Setup(& gWorld->floor_shader);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 255);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		nebu_Mesh_DrawGeometry( gWorld->floor );
		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
		video_Shader_Cleanup(& gWorld->floor_shader);
	} else {
		/* reflections */
		/* first draw reflector to stencil */
		/* and reflector alpha to fb */

		video_Shader_Setup(& gWorld->floor_shader);

		// store only reflector alpha in framebuffer
		glDepthMask(GL_FALSE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 255);
		glEnable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		// glEnable(GL_ALPHA_TEST);
		// glAlphaFunc(GL_GREATER, 0.1f);

		nebu_Mesh_DrawGeometry( gWorld->floor );
		
		// glDisable(GL_ALPHA_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 255);

		video_Shader_Cleanup(& gWorld->floor_shader);
		
		/* then draw world & skybox reflected, where stencil is set */
		/* protect the alpha buffer */
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

		isRenderingReflection = 1; // hack: reverse lighting
		glPushMatrix();
		glScalef(1,1,-1);
		glCullFace(GL_FRONT); // reverse culling
		// clip skybox & world to floor plane
		glEnable(GL_CLIP_PLANE0);
		{
			double plane[] = { 0, 0, 1, 0 };
			glClipPlane(GL_CLIP_PLANE0, plane);
		}

		drawSkybox( box2_Diameter( & game2->level->boundingBox ) * 2.5f );
		drawWorld(p, pV);

		glDisable(GL_CLIP_PLANE0);
		glCullFace(GL_BACK);
		glPopMatrix();
		isRenderingReflection = 0; // hack: normal lighting

		/* then blend the skybox into the scene, where stencil is set */
		/* modulate with the destination alpha */
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
		drawSkybox( box2_Diameter( & game2->level->boundingBox ) * 2.5f );
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		
		/* then blend reflector into the scene */
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(1, 1, 1, 1 - reflectivity);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_ALWAYS, 1, 255);

		video_Shader_Setup(& gWorld->floor_shader);
		nebu_Mesh_DrawGeometry( gWorld->floor );
		video_Shader_Cleanup(& gWorld->floor_shader);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	/* floor done */

	/* planar shadows */
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if(reflectivity != 1) // there are no shadows on perfect mirrors
		drawPlanarShadows(p);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	/* planar shadows done */

	drawWorld(p, pV);

	/* transparent stuff */
	/* draw the glow around the other players: */
	if (gSettingsCache.show_glow == 1) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (i = 0; i < game->players; i++)
		{
			if (p != game->player + i && PLAYER_IS_ACTIVE(game->player + i))
			{
				drawGlow(p->camera, game->player + i, gPlayerVisuals + i,
					d, TRAIL_HEIGHT * 4);
			}
		}
		glDisable(GL_BLEND);
	}
}
