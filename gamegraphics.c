#include "gltron.h"
#include "skybox.h"
#include "recognizer.h"
#include "explosion.h"

// static float arena[] = { 1.0, 1.2, 1, 0.0 };

#define MAX_LOD_LEVEL 3
static int lod_dist[MAX_LOD_LEVEL + 1][LC_LOD + 1] = { 
  { 1000, 1000, 1000 }, /* insane */
  { 25, 50, 250 }, /* high */
  { 5, 30, 200 }, /* low */
  { 1, 5, 150 } /* ugly */
};

/* spoke color */
static float SpokeColor[4] = {1.0, 1.0, 1.0, 1.0};
static float NoSpokeColor[4] = {0.0, 0.0, 0.0, 1.0};

void drawGame() {
  GLint i;
  Visual *d;
  Player *p;

  polycount = 0;

  glEnable(GL_DEPTH_TEST);

  glClearColor(game2->settingsCache.clear_color[0], 
               game2->settingsCache.clear_color[1], 
               game2->settingsCache.clear_color[2],
               game2->settingsCache.clear_color[3]);

  if(game2->settingsCache.use_stencil) {
    glClearStencil(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  } else {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  for(i = 0; i < vp_max[game->viewportType]; i++) {
    p = &(game->player[ viewport_content[i] ]);
    if(p->display->onScreen == 1) {
      d = p->display;
      glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
			drawCam(p, d);
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      if (game2->settingsCache.show_scores)
				drawScore(p, d);
      if (game2->settingsCache.show_ai_status)
				if(p->ai->active == AI_COMPUTER)
					drawAI(d);
    }
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
  }

  if (game2->settingsCache.show_fps)
    drawFPS(game->screen);

	if(game2->settingsCache.show_console)
		drawConsole(game->screen);

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
  return sqrt( scalarprod(tmp, tmp) );
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

void doCycleTurnRotation(Player *p) {
  int neigung_dir;
  unsigned int time = 0;
  float dirAngle;

  neigung_dir = -1.0;

  time = game2->time.current - p->data->turn_time;

  dirAngle = getDirAngle(time, p);
  glRotatef(dirAngle, 0, 0, 1);

#define neigung 25
  if(time < TURN_LENGTH) {
    float axis = 1.0;
    if(p->data->dir < p->data->last_dir && p->data->last_dir != 3)
      axis = -1.0;
    else if((p->data->last_dir == 3 && p->data->dir == 2) ||
	    (p->data->last_dir == 0 && p->data->dir == 3))
      axis = -1.0;
    glRotatef(neigung * sin(M_PI * time / TURN_LENGTH),
	      0.0, axis * neigung_dir, 0.0);
  }
#undef neigung
}

void drawCycleShadow(Player *p, int lod, int drawTurn) {
  Mesh *cycle;
  unsigned int turn_time;
  turn_time = game2->time.current - p->data->turn_time;
  if(turn_time < TURN_LENGTH && !drawTurn)
    return;

  if(p->data->exp_radius != 0)
    return;

  lod += game2->settingsCache.shadow_lod;
  if(lod > LC_LOD - 1) lod = LC_LOD - 1;

  cycle = lightcycle[lod];

  /* states */

  glEnable(GL_CULL_FACE);

  if(game2->settingsCache.use_stencil) {
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
  glTranslatef(p->data->posx, p->data->posy, 0.0);
  glMultMatrixf(shadow_matrix);
  if (game2->settingsCache.turn_cycle) {
    doCycleTurnRotation(p);
  } else if (p->data->exp_radius == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }
  glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

  /* render */

  drawModel(cycle, TRI_MESH);

  /* restore */

  if(game2->settingsCache.use_stencil)
    glDisable(GL_STENCIL_TEST);

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

void drawCycle(Player *p, int lod, int drawTurn) {
  Mesh *cycle;
  unsigned int  time, turn_time;
  cycle = lightcycle[lod];
    
  turn_time = game2->time.current - p->data->turn_time;
  if(turn_time < TURN_LENGTH && !drawTurn)
    return;

  time = game2->time.current  - p->data->spoke_time;
  
  glPushMatrix();
  glTranslatef(p->data->posx, p->data->posy, 0.0);

  if (p->data->exp_radius == 0 && game2->settingsCache.turn_cycle == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }

  if (game2->settingsCache.turn_cycle) { 
    doCycleTurnRotation(p);
  }

	setupLights(eCycles);
	
  SetMaterialColor(cycle, "Hull", eDiffuse, p->pColorDiffuse); 
  SetMaterialColor(cycle, "Hull", eSpecular, p->pColorSpecular); 

  if (time > (120 - (p->data->speed * 10))) {
    if (p->data->spoke_state == 1) {
      p->data->spoke_state = 0;
      SetMaterialColor(cycle, "Spoke", eAmbient, SpokeColor);
    } else {
      p->data->spoke_state = 1;
      SetMaterialColor(cycle, "Spoke", eAmbient, NoSpokeColor);
    }
    p->data->spoke_time = game2->time.current;
  }
  
  if (game2->settingsCache.light_cycles) {
    glEnable(GL_LIGHTING);
  }

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  if (p->data->exp_radius == 0) {
    glEnable(GL_NORMALIZE);

    glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

    glEnable(GL_CULL_FACE);
    drawModel(cycle, TRI_MESH);
    glDisable(GL_CULL_FACE);

  } else if(p->data->exp_radius < EXP_RADIUS_MAX) {
   
    glEnable(GL_BLEND);

    if (game2->settingsCache.show_impact) {
      drawImpact(p);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);

    drawModelExplosion(cycle, p->data->exp_radius);
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

  vsub(eye->camera->target, eye->camera->cam, v1);
  normalize(v1);
  tmp[0] = target->data->posx;
  tmp[1] = target->data->posy;
  tmp[2] = 0;

  lod_level = (game2->settingsCache.lod > MAX_LOD_LEVEL) ? 
    MAX_LOD_LEVEL : game2->settingsCache.lod;

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
  d = cos((game2->settingsCache.fov / 2) * 2 * M_PI / 360.0);
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

void drawPlayers(Player *p) {
  int i;

  for(i = 0; i < game->players; i++) {
		int lod;
		int drawTurn = 1;

		if (game2->settingsCache.camType == CAM_TYPE_COCKPIT && 
				p == &game->player[i])
			drawTurn = 0;

		lod = playerVisible(p, &(game->player[i]));
		if (lod >= 0) { 
			drawCycle(&(game->player[i]), lod, drawTurn);
		}
	}
}

void drawCam(Player *p, Visual *d) {
  int i;
  float up[3] = { 0, 0, 1 };
  
  glColor3f(0.0, 1.0, 0.0);
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  doPerspective(game2->settingsCache.fov, d->vp_w / d->vp_h,
                game2->settingsCache.znear, game2->rules.grid_size * 6.5);

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
  if (game2->settingsCache.show_skybox) {
    drawSkybox(game2->rules.grid_size);
  }

  /* fixme: clear z-buffer handling */
  /* glDepthMask(GL_TRUE); */
  
  /* floor */
  if (game2->settingsCache.show_floor_texture) {
    drawFloorTextured(game2->rules.grid_size,
                      game->screen->textures[TEX_FLOOR]);
  } else {
    /* should this be an artpack setting? */
    float line_color[] = {1.0, 1.0, 1.0};
    
    drawFloorGrid(game2->rules.grid_size,
                  game2->settingsCache.line_spacing,
                  line_color,
                  game2->settingsCache.clear_color);
  }
  
  /* glDepthMask(GL_FALSE); */

  /* shadows on the floor: cycle, recognizer, trails */
  if (game2->settingsCache.show_recognizer) {
    drawRecognizerShadow();
  }

  for(i = 0; i < game->players; i++) {
    int lod = playerVisible(p, game->player + i);
		if (lod >= 0) {
			int drawTurn = 1;
			if (! game2->settingsCache.camType == CAM_TYPE_COCKPIT ||
					p != &game->player[i])
				drawTurn = 0;
			drawCycleShadow(game->player + i, lod, drawTurn);
		}
		if (game->player[i].data->trail_height > 0 )
			drawTrailShadow(game->player + i);
	}
	
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  if (game2->settingsCache.show_recognizer &&
      p->data->speed != SPEED_GONE) {
    drawRecognizer();
  }

  if (game2->settingsCache.show_wall == 1) {
    drawWalls();
  }

  drawPlayers(p);

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
				trailGeometry(game->player + i, &mesh, &vOffset, &iOffset);
				bowGeometry(game->player + i, &mesh, &vOffset, &iOffset);
				trailStatesNormal(game->player + i, game->screen->textures[TEX_DECAL]);
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
			drawTrailLines(&(game->player[i]));

  /* transparent stuff */
  /* draw the glow around the other players: */
  if (game2->settingsCache.show_glow == 1) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (i = 0; i < game->players; i++) {
      if (p != game->player + i && PLAYER_IS_ACTIVE(game->player + i)) {
	      drawGlow(p, game->player + i, d, TRAIL_HEIGHT * 4);
      }
      
    glDisable(GL_BLEND);
    }
  }

	/* 2d hack */
	if(game2->settingsCache.map_ratio_w > 0)
	{
		Visual d2d;
		memcpy(&d2d, d, sizeof(Visual));
		d2d.vp_w *= game2->settingsCache.map_ratio_w;
		d2d.vp_h *= game2->settingsCache.map_ratio_h;

		d2d.vp_x += 20;
		d2d.vp_y += 20;
		
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		draw2D(&d2d);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}

void initGLGame() {
  glShadeModel( GL_SMOOTH );
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}
