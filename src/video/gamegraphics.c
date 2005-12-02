#include "video/video.h"
#include "video/trail_geometry.h"
#include "configuration/settings.h"
#include "base/nebu_vector.h"
#include "video/nebu_mesh.h"
#include "game/game.h"
#include "game/camera.h"
#include "game/game_level.h"
#include "game/resource.h"
#include "video/graphics_lights.h"
#include "video/graphics_utility.h"
#include "video/graphics_fx.h"
#include "video/graphics_hud.h"
#include "video/graphics_world.h"
#include "video/skybox.h"
#include "video/recognizer.h"
#include "video/explosion.h"

#include "video/nebu_video_system.h"
#include "video/nebu_renderer_gl.h"
#include "base/nebu_math.h"
#include "base/nebu_vector.h"
#include "base/nebu_matrix.h"

#include "base/nebu_debug_memory.h"

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

	clearScreen();

	glShadeModel( GL_SMOOTH );
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	for(i = 0; i < vp_max[gViewportType]; i++) {
		Visual *d = &gPlayerVisuals[viewport_content[i]].display;

		if(d->onScreen == 1) {
			glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
			drawCam(viewport_content[i]);

			/* hud stuff for every player */
			drawHUD(game->player + viewport_content[i],
				gPlayerVisuals + viewport_content[i]);
		}
	}
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

  if(p->data->exp_radius != 0)
    return;

  cycle = (gltron_Mesh*)resource_Get(gpTokenLightcycles[lod], eRT_GLtronTriMesh);

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
  } else if (p->data->exp_radius == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }
  glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

  /* render */

  gltron_Mesh_Draw(cycle, TRI_MESH);

  /* restore */
  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

void drawCycle(int player, int lod, int drawTurn) {
	Player *p = game->player + player;
	PlayerVisual *pV = gPlayerVisuals + player;

	gltron_Mesh *cycle = (gltron_Mesh*)resource_Get(gpTokenLightcycles[lod], eRT_GLtronTriMesh);
	float x, y;

	unsigned int spoke_time = game2->time.current - pV->spoke_time;
	int turn_time = game2->time.current - p->data->turn_time;

	if(turn_time < TURN_LENGTH && !drawTurn)
		return;

	getPositionFromData(&x, &y, p->data);
	glPushMatrix();
	glTranslatef(x, y, 0.0);

	if (p->data->exp_radius == 0 && gSettingsCache.turn_cycle == 0) {
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

	if (p->data->exp_radius == 0) {
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
#if 1
		if(p->data->wall_buster_enabled)
		{
			int i;
			float fScale;
			float gray[] = { 0.5f, 0.5f, 0.5f, 0.2f };
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			for(i = 0; i < 1; i++) {
				// fScale = 1 + (float)i * 0.2f / 5.0f;
				// gray[3] = 0.5f - 0.12f * i;
				fScale = 1.2f;
				gltron_Mesh_SetMaterialColor(cycle, "Hull", eDiffuse, gray);
				gltron_Mesh_SetMaterialAlpha(cycle, gray[3]);
				glPushMatrix();
				glScalef(fScale, fScale, fScale);
				gltron_Mesh_Draw(cycle, TRI_MESH);
				glPopMatrix();
				gltron_Mesh_SetMaterialAlpha(cycle, 1);
			}
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
#endif
		glDisable(GL_CULL_FACE);

		glDisable(GL_LIGHTING);
	} else if(p->data->exp_radius < EXP_RADIUS_MAX) {
		glEnable(GL_BLEND);

		if (gSettingsCache.show_impact) {
			drawImpact(player);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

		if (gSettingsCache.light_cycles) {
			glEnable(GL_LIGHTING); // enable OpenGL lighting for lightcycles
		}
		gltron_Mesh_DrawExplosion(cycle, p->data->exp_radius);
		glDisable(GL_LIGHTING); // disable ligthing after lightcycles
		glDisable(GL_BLEND);
	}
	glPopMatrix();
}
 
int playerVisible(int eyePlayer, int targetPlayer) {
	vec3 v1, v2, tmp;
		
	float s;
	float d;
	int i;
	int lod_level;
	float x, y;

	vec3_Sub(&v1, (vec3*) gPlayerVisuals[eyePlayer].camera.target, (vec3*) gPlayerVisuals[eyePlayer].camera.cam);
	vec3_Normalize(&v1, &v1);
	
	getPositionFromData(&x, &y, game->player[targetPlayer].data);
	tmp.v[0] = x;
	tmp.v[1] = y;
	tmp.v[2] = 0;
		
	lod_level = (gSettingsCache.lod > MAX_LOD_LEVEL) ? 
		MAX_LOD_LEVEL : gSettingsCache.lod;

	/* calculate lod */
	vec3_Sub(&v2, (vec3*) &gPlayerVisuals[eyePlayer].camera.cam, &tmp);
	d = vec3_Length(&v2);
	for(i = 0; i < LC_LOD && d >= lod_dist[lod_level][i]; i++);
	if(i >= LC_LOD)
		return -1;

	vec3_Sub(&v2, &tmp, (vec3*) gPlayerVisuals[eyePlayer].camera.cam);
	vec3_Normalize(&v2, &v2);
	s = vec3_Dot(&v1, &v2);
	/* maybe that's not exactly correct, but I didn't notice anything */
	d = cosf((gSettingsCache.fov / 2) * 2 * PI / 360.0);
	/*
		printf("v1: %.2f %.2f %.2f\nv2: %.2f %.2f %.2f\ns: %.2f d: %.2f\n\n",
		v1[0], v1[1], v1[2], v2[0], v2[1], v2[2],
		s, d);
	*/
	if(s < d-(((gltron_Mesh*)resource_Get(gpTokenLightcycles[i], eRT_GLtronTriMesh))->BBox.fRadius*2))
		return -1;
	else
		return i;
}

void drawPlayers(int player) {
  int i;

  for(i = 0; i < game->players; i++) {
		int lod;
		int drawTurn = 1;

		if (i == player &&
			gSettingsCache.camType == CAM_TYPE_COCKPIT)
			drawTurn = 0;

		lod = playerVisible(player, i);
		if (lod >= 0) { 
			drawCycle(i, lod, drawTurn);
		}
	}
}

void drawPlanarShadows(int player) {
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
		int lod = playerVisible(player, i);
		if (lod >= 0) {
			int drawTurn = (i != player || gSettingsCache.camType != CAM_TYPE_COCKPIT) ? 1 : 0;
			drawCycleShadow(gPlayerVisuals + i, game->player + i, lod, drawTurn);
		}
		if (game->player[i].data->trail_height > 0 )
			drawTrailShadow(game->player + i, gPlayerVisuals + i);
	}

	if(gSettingsCache.use_stencil)
		glDisable(GL_STENCIL_TEST);

	glDisable(GL_BLEND);
}

void drawWorld(int player) {
	int i;

	nebu_Video_CheckErrors("before world");

	setupLights(eWorld);

	if (gSettingsCache.show_recognizer &&
		game->player[player].data->speed != SPEED_GONE) {
		drawRecognizer();
	}

	if (gSettingsCache.show_wall == 1) {
		glColor3f(1,1,1);
		drawWalls();
	}

	setupLights(eCycles);

	drawPlayers(player);

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

/*	draw's a GLtron scene, which consists of the following step:
	- setup up a perspective projection matrix
	- setup the view matrix
	- draw the skybox
	- draw the floor geometry, possible with blended world & skybox reflections
	- draw projected shadows of the trails, lightcycles & recognizer to the
	  floor geometry (darkening pass)
	- draw world geometry (recognizer, arena walls, lightcycles,
	  cycle trails (back-to-front)
*/

/*	TODO: with engine style rendering, that's
	- old style rendering
		- foreach object in the scene
			- find geometry
			- find shader
			- draw shader
		- objects consist of:
			- multiple sets of:
				- mesh (vertex & perhaps index buffer)  
				- world transformation
		- example 1 (without reflections or shadows):
			skybox:
				- 6 objects (quaads, i.e. two triangles)
				- 6 shaders of the same type (but with a different texture each)
			floor:
				- 1 object & shader (one texture)
			walls:
				- 1 object & shader (one texture)
			lightcycles & recognizer (not finalized):
				- 1 object per material & shader (no texture, but with special lighting,
					wheel spoke stuff, explosions, recognizer outlines,
					etc.)
			lightcycle trail:
				- TODO
			special effects:
				- lightcycle trail glow
				- trail lines
		- example 2 (without reflections, but with projected shadows)
			- same as example 2, but add after floor:
				- all object geometry that casts a drop-shadow (careful!
				 what about objects that rely on alpha-transparency?)
				- drop shadow shader (setup projection matrix, darkening)

	2D post processing happens after all viewports are drawn (i.e.
	at the end of the drawGame() function, and consists of
	- HUD
	- full screen effects
*/

/* plan for the transition:
	move objects 1-by-1 into the 'scenegraph' (temporarily screws up
	drawing order, but so what)
	traverse scenegraph each frame, and build object & shader lists
*/


void drawCam(int player) {
	int i;
	float up[3] = { 0, 0, 1 };
	Visual *d = & gPlayerVisuals[player].display;
	
	float reflectivity = getReflectivity();
	// compute shadow color based on glocal constant & reflectivity
	for(i = 0; i < 4; i++) 
		gCurrentShadowColor[i] = gShadowColor[i] * (1 - reflectivity);

	// setup up a perspective projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	doPerspective(gSettingsCache.fov, (float) d->vp_w / (float) d->vp_h,
		gSettingsCache.znear, box2_Diameter(& game2->level->boundingBox) * 6.5f);

	// setup the view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	{
		vec3 vLookAt;
		vec3 vTarget;
		matrix matRotate;

		vec3_Sub(&vLookAt, (vec3*)gPlayerVisuals[player].camera.target, (vec3*)gPlayerVisuals[player].camera.cam);
		vec3_Normalize(&vLookAt, &vLookAt);
		matrixRotationAxis(&matRotate, 90.0f * (float) gPlayerVisuals[player].camera.bIsGlancing, (vec3*)up);
		vec3_Transform(&vLookAt, &vLookAt, &matRotate);
		vec3_Add(&vTarget, (vec3*)gPlayerVisuals[player].camera.cam, &vLookAt);
		doLookAt(gPlayerVisuals[player].camera.cam, (float*)&vTarget, up);
	}

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
		drawWorld(player);

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
		drawPlanarShadows(player);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	/* planar shadows done */

	drawWorld(player);

	/* transparent stuff */
	/* draw the glow around the other players: */
	if (gSettingsCache.show_glow == 1) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (i = 0; i < game->players; i++)
		{
			if (i != player && PLAYER_IS_ACTIVE(game->player + i))
			{
				drawGlow(&gPlayerVisuals[player].camera, game->player + i, gPlayerVisuals + i,
					d, TRAIL_HEIGHT * 4);
			}
		}
		glDisable(GL_BLEND);
	}
}
