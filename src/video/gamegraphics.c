#include "video/video.h"
#include "game/game.h"

#include "video/skybox.h"
#include "video/recognizer.h"
#include "video/explosion.h"

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

void drawGame(void) {
  GLint i;

  polycount = 0;

  glEnable(GL_DEPTH_TEST);

  glClearColor(gSettingsCache.clear_color[0], 
               gSettingsCache.clear_color[1], 
               gSettingsCache.clear_color[2],
               gSettingsCache.clear_color[3]);

  if(gSettingsCache.use_stencil) {
    glClearStencil(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  } else {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  for(i = 0; i < vp_max[gViewportType]; i++) {
		Player *p = game->player + viewport_content[i];
		PlayerVisual *pV = gPlayerVisuals + viewport_content[i];
		Visual *d = & pV->display;
	
    if(d->onScreen == 1) {
      glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
			drawCam(p, pV);
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      if (gSettingsCache.show_scores)
				drawScore(p, d);
      if (gSettingsCache.show_ai_status)
				if(p->ai->active == AI_COMPUTER)
					drawAI(d);
    }
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
  }

  if (gSettingsCache.show_fps)
    drawFPS(gScreen);

	if(gSettingsCache.show_console)
		drawConsole(gScreen);

  /* printf("%d polys\n", polycount); */
}
 
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
  Mesh *cycle;
  int turn_time = game2->time.current - p->data->turn_time;
      
  if(turn_time < TURN_LENGTH && !drawTurn)
    return;

  if(pV->exp_radius != 0)
    return;

  cycle = lightcycle[lod];

  /* states */

  glEnable(GL_CULL_FACE);

  if(gSettingsCache.use_stencil) {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_GREATER, 1, 1);
    glEnable(GL_BLEND);
    glColor4fv(shadow_color);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else {
    glColor3f(0, 0, 0);
    glDisable(GL_BLEND);
  }

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

  drawModel(cycle, TRI_MESH);

  /* restore */

  if(gSettingsCache.use_stencil)
    glDisable(GL_STENCIL_TEST);

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

void drawCycle(Player *p, PlayerVisual *pV, int lod, int drawTurn) {
  Mesh *cycle = lightcycle[lod];

  unsigned int spoke_time = game2->time.current - pV->spoke_time;
	int turn_time = game2->time.current - p->data->turn_time;

  if(turn_time < TURN_LENGTH && !drawTurn)
    return;
  
  glPushMatrix();
	{
		float x, y;
		getPositionFromData(&x, &y, p->data);
		glTranslatef(x, y, 0.0);
	}

  if (pV->exp_radius == 0 && gSettingsCache.turn_cycle == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }

  if (gSettingsCache.turn_cycle) { 
    doCycleTurnRotation(pV, p);
  }

	setupLights(eCycles);
	
  SetMaterialColor(cycle, "Hull", eDiffuse, pV->pColorDiffuse); 
  SetMaterialColor(cycle, "Hull", eSpecular, pV->pColorSpecular); 

  if (gSettingsCache.light_cycles) {
    glEnable(GL_LIGHTING);
  }

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  if (pV->exp_radius == 0) {
    glEnable(GL_NORMALIZE);

    glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

    /* draw spoke animation */
    if (spoke_time > 140 - (p->data->speed * 10) 
        && game->pauseflag == PAUSE_GAME_RUNNING) {
      if (pV->spoke_state == 1) {
        pV->spoke_state = 0;
        SetMaterialColor(cycle, "Spoke", eSpecular, SpokeColor);
        SetMaterialColor(cycle, "Spoke", eAmbient, SpokeColor);
      } else {
        pV->spoke_state = 1;
        SetMaterialColor(cycle, "Spoke", eSpecular, NoSpokeColor);
        SetMaterialColor(cycle, "Spoke", eAmbient, NoSpokeColor);
      }
      pV->spoke_time = game2->time.current;
    }
    
    glEnable(GL_CULL_FACE);
    drawModel(cycle, TRI_MESH);
    glDisable(GL_CULL_FACE);

  } else if(pV->exp_radius < EXP_RADIUS_MAX) {
   
    glEnable(GL_BLEND);

    if (gSettingsCache.show_impact) {
      drawImpact(pV);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

    drawModelExplosion(cycle, pV->exp_radius);
  }
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glPopMatrix();
}
 
int playerVisible(Player *eye, Player *target) {
  float v1[3];
  float v2[3];
  float tmp[3];
  float s;
  float d;
  int i;
  int lod_level;
	float x, y;

  vsub(eye->camera->target, eye->camera->cam, v1);
  normalize(v1);
	
	getPositionFromData(&x, &y, target->data);
	tmp[0] = x;
  tmp[1] = y;
  tmp[2] = 0;
	
  lod_level = (gSettingsCache.lod > MAX_LOD_LEVEL) ? 
    MAX_LOD_LEVEL : gSettingsCache.lod;

  /* calculate lod */
  vsub(eye->camera->cam, tmp, v2);
  d = length(v2);
  for(i = 0; i < LC_LOD && d >= lod_dist[lod_level][i]; i++);
  if(i >= LC_LOD)
    return -1;

  vsub(tmp, eye->camera->cam, v2);
  normalize(v2);
  s = scalarprod(v1, v2);
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

void drawCam(Player *p, PlayerVisual* pV) {
  int i;
  float up[3] = { 0, 0, 1 };
	Visual *d = & pV->display;
  
  glColor3f(0.0, 1.0, 0.0);
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  doPerspective(gSettingsCache.fov, d->vp_w / d->vp_h,
                gSettingsCache.znear, game2->rules.grid_size * 6.5f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /* set positions for GL lights in world coordinates */
  glLightfv(GL_LIGHT1, GL_POSITION, p->camera->cam);

  doLookAt(p->camera->cam, p->camera->target, up);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  /* skybox */
  if (gSettingsCache.show_skybox) {
    drawSkybox(game2->rules.grid_size);
  }

  /* fixme: clear z-buffer handling */
  /* glDepthMask(GL_TRUE); */
  
  /* floor */
  if (gSettingsCache.show_floor_texture) {
    drawFloorTextured(game2->rules.grid_size,
                      gScreen->textures[TEX_FLOOR]);
  } else {
    /* should this be an artpack setting? */
    float line_color[] = {1.0, 1.0, 1.0};
    
    drawFloorGrid(game2->rules.grid_size,
                  gSettingsCache.line_spacing,
                  line_color,
                  gSettingsCache.clear_color);
  }
  
  /* glDepthMask(GL_FALSE); */

  /* shadows on the floor: cycle, recognizer, trails */
  if (gSettingsCache.show_recognizer) {
    drawRecognizerShadow();
  }

  for(i = 0; i < game->players; i++) {
    int lod = playerVisible(p, game->player + i);
		if (lod >= 0) {
			int drawTurn = 1;
			if (! gSettingsCache.camType == CAM_TYPE_COCKPIT ||
	 			p != &game->player[i])
				drawTurn = 0;
			drawCycleShadow(gPlayerVisuals + i, game->player + i, lod, drawTurn);
		}
		if (game->player[i].data->trail_height > 0 )
			drawTrailShadow(game->player + i, gPlayerVisuals + i);
	}
	
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  if (gSettingsCache.show_recognizer &&
      p->data->speed != SPEED_GONE) {
    drawRecognizer();
  }

  if (gSettingsCache.show_wall == 1) {
    drawWalls();
  }

  drawPlayers(p, pV);

	setupLights(eWorld);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);

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

  glDisable(GL_POLYGON_OFFSET_FILL);

  for(i = 0; i < game->players; i++)
    if (game->player[i].data->trail_height > 0 )
			drawTrailLines(game->player + i, gPlayerVisuals + i);

  /* transparent stuff */
  /* draw the glow around the other players: */
  if (gSettingsCache.show_glow == 1) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (i = 0; i < game->players; i++) {
      if (p != game->player + i && PLAYER_IS_ACTIVE(game->player + i)) {
	      drawGlow(p->camera, game->player + i, gPlayerVisuals + i,
								 d, TRAIL_HEIGHT * 4);
      }
      
    glDisable(GL_BLEND);
    }
  }
	/* 2d hack */
	if(gSettingsCache.map_ratio_w > 0)
	{
		Visual d2d;
		memcpy(&d2d, d, sizeof(Visual));
		d2d.vp_w *= gSettingsCache.map_ratio_w;
		d2d.vp_h *= gSettingsCache.map_ratio_h;

		d2d.vp_x += 20;
		d2d.vp_y += 20;
		
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		draw2D(&d2d);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}

void initGLGame(void) {
  glShadeModel( GL_SMOOTH );
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}
