#include "gltron.h"
#include "skybox.h"
#include "recognizer.h"
#include "floor.h"

static float arena[] = { 1.0, 1.2, 1, 0.0 };

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
  gDisplay *d;
  Player *p;

  polycount = 0;

  glEnable(GL_DEPTH_TEST);

  glClearColor(game2->settingsCache.clear_color[0], 
               game2->settingsCache.clear_color[1], 
               game2->settingsCache.clear_color[2],
               game2->settingsCache.clear_color[3]);

#ifdef DO_STENCIL
  glClearStencil(0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
#else
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
#endif

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

  if (game2->settingsCache.show_2d > 0) {
    drawDebugTex(game->screen);
    drawDebugLines(game->screen);
  }
  if (game2->settingsCache.show_fps)
    drawFPS(game->screen);

  drawConsole(game->screen);
  /*
    if(getSettingi("show_help") == 1)
    drawHelp(game->screen);
  */

  /* printf("%d polys\n", polycount); */
}

void checkGLError(char *where) {
  int error;
  error = glGetError();
  if(error != GL_NO_ERROR)
    printf("[glError: %s] - %d\n", where, error);
}

void rasonly(gDisplay *d) {
  /* do rasterising only (in local display d) */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, (GLfloat) d->vp_w, 0.0f, (GLfloat) d->vp_h, 0.0f, 1.0f);
  checkGLError("rasonly");
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
}

void drawText(fonttex* ftx, int x, int y, int size, char *text) {
#if 0
  if(getSettingi("softwareRendering")) {
    drawSoftwareText(ftx, x, y, size, text);
  } else {
#endif 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(size, size, size);
    ftxRenderString(ftx, text, strlen(text));
  
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    polycount += 2 * strlen(text); /* quads are two triangles */
#if 0
  }
#endif
}

void initModelLights(int light) {
  /* float col[] = { .77, .77, .77, 1.0 }; */
  float col[] = { .95, .95, .95, 1.0 };
  float dif[] =  { 0.4, 0.4, 0.4, 1};
  float amb[] = { 0.05, 0.05, 0.05, 1};

  glLightfv(light, GL_AMBIENT, amb);
  glLightfv(light, GL_SPECULAR, col);
  glLightfv(light, GL_DIFFUSE, dif);
  
}
 
void initTrailLights(int light) {
  /* doing my own lighting for that now */
  /* int light is currently ignored */
  float amb[] = { 0.2, 0.2, 0.2, 1};
  setFactor3fv(amb);
  setLight4fv(arena);
}

void rebuildDebugTex() {
  int x, y;
  int px = -1, py = -1;
  float tx, ty;
  int color;
  unsigned char *source;
  /* printf("rebuilding texture: %d -> ", SystemGetElapsedTime()); */
  for(y = 0; y < game2->rules.grid_size; y++) {
    ty = (float) y * DEBUG_TEX_H / game2->rules.grid_size;
    for(x = 0; x < game2->rules.grid_size; x++) {
      tx = (float) x * DEBUG_TEX_W / game2->rules.grid_size;
      color = colmap [ y * colwidth + x];
      if(color != 0 || ((int)tx != px && (int)ty != py)) {
	px = (int) tx; 
	source = debugcolors[ color ];
	memcpy(debugtex + (int)ty * DEBUG_TEX_W * 4 + px * 4, source, 4);
      }
    }
    py = (int)ty;
  }
  /* printf("%d\n", SystemGetElapsedTime()); */
}

#define MAP_SIZE 256.0

void drawDebugLines(gDisplay *d) {
  int i;
  Player *p;
  Data *data;
  line *line;
  int size;
  float scale;

  rasonly(d);
  glTranslatef(10, 400, 0);
  size = game2->rules.grid_size;

  scale = MAP_SIZE / size;

  glPushMatrix();

  glScalef(scale, scale, scale);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINE_LOOP);
  glVertex2i(0, 0);
  glVertex2i(size, 0);
  glVertex2i(size, size);
  glVertex2i(0, size);
  glEnd();

  /* glLineWidth(2); */ /* buggy in Mesa/Glide */
  for(i = 0; i < game->players; i++) {
    p = &(game->player[i]);
    data = p->data;
    if(PLAYER_IS_ACTIVE(p)) {
      glBegin(GL_LINES);
      glColor3fv(p->pColorAlpha);
      line = &(data->trails[0]);
      while(line != data->trail) {

	glVertex2f(line->sx, line->sy);
	glVertex2f(line->ex, line->ey);
	line++;
	polycount++;
      }
      glVertex2f(line->sx, line->sy);
      glVertex2f(data->posx, data->posy);
      glEnd();
    }
  }
  /* glLineWidth(1); */
  glPopMatrix();
}

void drawDebugTex(gDisplay *d) {
  glDisable(GL_DEPTH_TEST);
  /* build2DTex(); */
  /* fprintf(stderr, "%d ", SystemGetElapsedTime()); */
  rasonly(d);
  glTranslatef(10, 100, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*
    glBegin(GL_QUADS);
    glColor4f(1.0, 1.0, 1.0, 0.1);
    glTexCoord2f(0.0, 0.0); glVertex2i(0, 0);
    glTexCoord2f(1.0, 0.0); glVertex2i(255, 0);
    glTexCoord2f(1.0, 1.0); glVertex2i(255, 255);
    glTexCoord2f(0.0, 1.0); glVertex2i(0, 255);  
    glEnd();
  */

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, d->textures[TEX_DEBUG]);
  if(ogl_debugtex == 0) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DEBUG_TEX_W, DEBUG_TEX_H, 
		 0, GL_RGBA, GL_UNSIGNED_BYTE, debugtex);
    ogl_debugtex = 1;
  } else {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DEBUG_TEX_W, DEBUG_TEX_H, GL_RGBA,
		    GL_UNSIGNED_BYTE, debugtex);
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glBlendFunc(GL_ONE, GL_ONE);
  glBegin(GL_QUADS);
  glColor3f(1.0, 1.0, 1.0);
  glTexCoord2f(0.0, 0.0); glVertex2i(0, 0);
  glTexCoord2f(1.0, 0.0); glVertex2i(255, 0);
  glTexCoord2f(1.0, 1.0); glVertex2i(255, 255);
  glTexCoord2f(0.0, 1.0); glVertex2i(0, 255);  
  glEnd();

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINE_LOOP);
  glVertex2i(0, 0);
  glVertex2i(255, 0);
  glVertex2i(255, 255);
  glVertex2i(0, 255);
  glEnd();

  /*
    int x = 100;
    int y = 100;

    rasonly(d);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glColor4f(.0, 1.0, .0, 1.0);
    glRasterPos2i(x, y);
    glBitmap(colwidth * 8 , game2->rules.grid_size, 0, 0, 0, 0, colmap);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x - 1, y - 1);
    glVertex2i(x + colwidth * 8, y - 1);
    glVertex2i(x + colwidth * 8, y + game2->rules.grid_size);
    glVertex2i(x - 1, y + game2->rules.grid_size);
    glEnd();
    polycount += 4;
  */
  /* fprintf(stderr, "%d\n", SystemGetElapsedTime()); */
  glEnable(GL_DEPTH_TEST);
}

void drawConsoleLines(char *line, int call) {
#define CONSOLE_SIZE 15
#define CONSOLE_X_OFF 20
  int size = CONSOLE_SIZE;
  int length;
  /* fprintf(stdout, "%s\n", line); */
  length = strlen(line);
  while(length * size > game->screen->vp_w / 2 - CONSOLE_X_OFF)
    size--;
    
  if(*line != 0) 
    drawText(gameFtx, CONSOLE_X_OFF, game->screen->vp_h - 20 * (call + 1),
	     size, line);
}

void drawConsole(gDisplay *d) {
  int lines;
  rasonly(d);
  glColor3f(1.0, 0.3, 0.3);
  
  if (game2->settingsCache.softwareRendering) { 
    lines = 1;
  } else if (game->screen->vp_h < 600) {
    lines = 3;
  } else {
    lines = 5;
  }
  
  consoleDisplay(drawConsoleLines, lines);
}
  
void drawFPS(gDisplay *d) {
#define FPS_HSIZE 20
  /* draws FPS in upper left corner of Display d */
  static int fps_h[FPS_HSIZE];
  static int pos = -FPS_HSIZE;
  static int fps_min = 0;
  static int fps_avg = 0;

  char tmp[20];
  int diff;

  rasonly(d);
  diff = (game2->time.dt > 0) ? game2->time.dt : 1;

  if(pos < 0) {
    fps_avg = 1000 / diff;
    fps_min = 1000 / diff;
    fps_h[pos + FPS_HSIZE] = 1000 / diff;
    pos++;
  } else {
    fps_h[pos] = 1000 / diff;
    pos = (pos + 1) % FPS_HSIZE;
    if(pos % 10 == 0) {
      int i;
      int sum = 0;
      int min = 1000;
      for(i = 0; i < FPS_HSIZE; i++) {
	sum += fps_h[i];
	if(fps_h[i] < min)
	  min = fps_h[i];
      }
      fps_min = min;
      fps_avg = sum / FPS_HSIZE;
    }
  }

  sprintf(tmp, "average FPS: %d", fps_avg);
  glColor4f(1.0, 0.4, 0.2, 1.0);
  drawText(gameFtx, d->vp_w - 180, d->vp_h - 20, 10, tmp);
  sprintf(tmp, "minimum FPS: %d", fps_min);
  drawText(gameFtx, d->vp_w - 180, d->vp_h - 35, 10, tmp);
  sprintf(tmp, "triangles: %d", polycount);
  drawText(gameFtx, d->vp_w - 180, d->vp_h - 50, 10, tmp);
}

void drawScore(Player *p, gDisplay *d) {
  char tmp[10]; /* hey, they won't reach such a score */

  sprintf(tmp, "%d", p->data->score);
  rasonly(d);
  glColor4f(1.0, 1.0, 0.2, 1.0);
  drawText(gameFtx, 5, 5, 32, tmp);
}

/* distance from point v to line p + t * d */
 
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

void drawCrash(float radius) {
#define CRASH_W 32
#define CRASH_H 16
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glColor4f(1.0, 1.0, 1.0, (EXP_RADIUS_MAX - radius) / EXP_RADIUS_MAX);
  /* printf("exp_r: %.2f\n", (EXP_RADIUS_MAX - radius) / EXP_RADIUS_MAX); */
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_CRASH]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_BLEND);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(- CRASH_W / 2, 0.0, 0.0);
  glTexCoord2f(1.0, 0.0);
  glVertex3f(CRASH_W / 2, 0.0, 0.0);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(CRASH_W / 2, 0.0, CRASH_H);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(- CRASH_W / 2, 0.0, CRASH_H);
  glEnd();

  polycount += 2;

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
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

void drawCycleShadow(Player *p, int lod) {
  Mesh *cycle;

  lod += game2->settingsCache.shadow_lod;
  if(lod > LC_LOD - 1) lod = LC_LOD - 1;

  cycle = lightcycle[lod];
  glPushMatrix();
  glTranslatef(p->data->posx, p->data->posy, 0.0);

  glMultMatrixf(shadow_matrix);
  if (game2->settingsCache.turn_cycle) {
    doCycleTurnRotation(p);
  } else if (p->data->exp_radius == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glPolygonOffset(1, 1);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* fixme: 
   * use glColor4fv(shadow_color); instead, but
   * watch for z-buffer artefacts
   */
  glColor3f(0, 0, 0);

  /* lightcycle local transformation */
  /* glScalef(10, 10, 10); */
  glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);
  /* glRotatef(90, 0, 0, 1); */

  if(p->data->exp_radius == 0) {
    glEnable(GL_CULL_FACE);
    drawModel(cycle, TRI_MESH);
    glDisable(GL_CULL_FACE);
  }

  glPolygonOffset(0, 0);
  glPopMatrix();
}

void drawCycle(Player *p, int lod) {
  Mesh *cycle;
  unsigned int  time;
  cycle = lightcycle[lod];
    
  time = game2->time.current  - p->data->spoke_time;
  
  glPushMatrix();
  glTranslatef(p->data->posx, p->data->posy, 0.0);

  if (game2->settingsCache.show_crash_texture) {
    if (p->data->exp_radius > 0 && p->data->exp_radius < EXP_RADIUS_MAX) {
      glPushMatrix();
      glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
      drawCrash(p->data->exp_radius);
      glPopMatrix();
    }
  }

  if (p->data->exp_radius == 0 && game2->settingsCache.turn_cycle == 0) {
    glRotatef(dirangles[p->data->dir], 0.0, 0.0, 1.0);
  }

  if (game2->settingsCache.turn_cycle) { 
    doCycleTurnRotation(p);
  }

  initModelLights(GL_LIGHT1);
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

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

    /* glScalef(10, 10, 10); */
    glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);
    /* glRotatef(90, 0, 0, 1); */

    glEnable(GL_CULL_FACE);
    drawModel(cycle, TRI_MESH);
    glDisable(GL_CULL_FACE);
  }
  else if(p->data->exp_radius < EXP_RADIUS_MAX) {
    float alpha;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    alpha = (float) (EXP_RADIUS_MAX - p->data->exp_radius) /
      (float) EXP_RADIUS_MAX;
#if 0
    setMaterialAlphas(cycle, alpha);
#endif
    /* glScalef(10, 10, 10); */
    glTranslatef(0, 0, cycle->BBox.vSize.v[2] / 2);
    /* glRotatef(90, 0, 0, 1); */

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
  float height;
  int lod;

  for(i = 0; i < game->players; i++) {
    height = game->player[i].data->trail_height;

    if (game2->settingsCache.show_model) {
      lod = playerVisible(p, &(game->player[i]));
      if (lod >= 0) { 
	drawCycle(&(game->player[i]), lod);
      }
    }
  }
}

void drawGlow(Player *p, gDisplay *d, float dim) {
  float mat[4*4];
  
  glPushMatrix();
  glTranslatef(p->data->posx,
	       p->data->posy,
	       0);

  glShadeModel(GL_SMOOTH);

  glDepthMask(GL_FALSE);
  glEnable(GL_DEPTH_TEST);

  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);

  glGetFloatv(GL_MODELVIEW_MATRIX, mat);
  mat[0] = mat[5] = mat[10] = 1.0;
  mat[1] = mat[2] = 0.0;
  mat[4] = mat[6] = 0.0;
  mat[8] = mat[9] = 0.0;
  glLoadMatrixf(mat);
  glBegin(GL_TRIANGLE_FAN);
  glColor3fv(p->pColorDiffuse);

  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(dim*cos(-0.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(-0.2*3.1415/5.0), 0);
  glVertex3f(dim*cos(1.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(1.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(2.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(2.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(3.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(3.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(4.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(4.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(5.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(5.2*3.1415/5.0), 0);
  glEnd();
  polycount += 5;


  glBegin(GL_TRIANGLES);
  glColor3fv(p->pColorDiffuse);
  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(0,-TRAIL_HEIGHT/4,0);
  glVertex3f(dim*cos(-0.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(-0.2*3.1415/5.0), 0);

  glColor3fv(p->pColorDiffuse);
  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(dim*cos(5.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(5.2*3.1415/5.0), 0);
  glVertex3f(0,-TRAIL_HEIGHT/4,0);
  glEnd();
  polycount += 3;

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glShadeModel( game->screen->shademodel );
  glPopMatrix();  


}

void drawWalls() {
#undef WALL_H
#define WALL_H 48
  float t;
  float h;

  t = game2->rules.grid_size / 240.0;
  if (game2->settingsCache.stretch_textures) {
    h = t * WALL_H;
    t = 1.0;
  } else h = WALL_H;

  glColor4f(1.0, 1.0, 1.0, 1.0);


  /*
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  */
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
#define T_TOP 1.0
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_WALL1]);
  glBegin(GL_QUADS);
  glTexCoord2f(t, 0.0); glVertex3f(0.0, 0.0, 0.0);
  glTexCoord2f(t, T_TOP); glVertex3f(0.0, 0.0, h);
  glTexCoord2f(0.0, T_TOP); glVertex3f(game2->rules.grid_size, 0.0, h);
  glTexCoord2f(0.0, 0.0); glVertex3f(game2->rules.grid_size, 0.0, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_WALL2]);
  glBegin(GL_QUADS);
  glTexCoord2f(t, 0.0); glVertex3f(game2->rules.grid_size, 0.0, 0.0);
  glTexCoord2f(t, T_TOP); glVertex3f(game2->rules.grid_size, 0.0, h);
  glTexCoord2f(0.0, T_TOP); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, h);
  glTexCoord2f(0.0, 0.0); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_WALL3]);
  glBegin (GL_QUADS);
  glTexCoord2f(t, 0.0); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, 0.0);
  glTexCoord2f(t, T_TOP); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, h);
  glTexCoord2f(0.0, T_TOP); glVertex3f(0.0, game2->rules.grid_size, h);
  glTexCoord2f(0.0, 0.0); glVertex3f(0.0, game2->rules.grid_size, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_WALL4]);
  glBegin(GL_QUADS);
  glTexCoord2f(t, 0.0); glVertex3f(0.0, game2->rules.grid_size, 0.0);
  glTexCoord2f(t, T_TOP); glVertex3f(0.0, game2->rules.grid_size, h);
  glTexCoord2f(0.0, T_TOP); glVertex3f(0.0, 0.0, h);
  glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0); 
#undef T_TOP
  glEnd();
  polycount += 8;

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
}


void doPerspective(float fov, float ratio, float znear, float zfar) {
  float top;
  float left;

  top = tan( fov * M_PI / 360.0 ) * znear;
  left = - top * ratio;
  glFrustum(left, -left, -top, top, znear, zfar);
}

void doLookAt(float *cam, float *target, float *up) {
  float m[16];
  float x[3], y[3], z[3];

  vsub(cam, target, z);
  normalize(z);
  crossprod(up, z, x);
  crossprod(z, x, y);
  normalize(x);
  normalize(y);

#define M(row,col)  m[col*4+row]
  M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
  M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
  M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
  M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
  glMultMatrixf( m );

  /* Translate Eye to Origin */
  glTranslatef( -cam[0], -cam[1], -cam[2]);
}


void drawCam(Player *p, gDisplay *d) {
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
  for(i = 0; i < game->players; i++) {
    int lod;
    if (game2->settingsCache.show_model) {
      lod = playerVisible(p, game->player + i);
      if (lod >= 0) {
	drawCycleShadow(game->player + i, lod);
      }
    }
    drawTrailShadow(game->player + i);
  }

  if (game2->settingsCache.show_recognizer) {
    drawRecognizerShadow();
  }

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  if (game2->settingsCache.show_recognizer) {
    drawRecognizer();
  }

  if (game2->settingsCache.show_wall == 1) {
    drawWalls(d);
  }

  drawPlayers(p);

  glShadeModel(game->screen->shademodel);

  initTrailLights(0);


  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);
  glShadeModel(GL_SMOOTH);

  doTrails(p);
  glShadeModel(game->screen->shademodel);
  glDisable(GL_POLYGON_OFFSET_FILL);

  for(i = 0; i < game->players; i++)
    drawTrailLines(&(game->player[i]));

  /* transparent stuff */
  /* draw the glow around the other players: */
  if (game2->settingsCache.show_glow == 1) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (i = 0; i < game->players; i++) {
      if ((p != &(game->player[i])) && PLAYER_IS_ACTIVE(&game->player[i])) {
	      drawGlow(&(game->player[i]), d, TRAIL_HEIGHT * 4);
      }
      
    glDisable(GL_BLEND);
    }
  }

  /*if(getSettingi("show_recognizer")) {
#if DEPTH_CLEAR
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
#endif
    glDepthMask(GL_TRUE);

#if DEPTH_CLEAR
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    doPerspective(getSettingi("fov"), d->vp_w / d->vp_h,
		  getSettingi("znear") * 2, game2->rules.grid_size * 6.5);

    glMatrixMode(GL_MODELVIEW);
#endif

    glEnable(GL_CULL_FACE);
    drawRecognizers(1);
    glDisable(GL_CULL_FACE);
  }*/
}

void drawAI(gDisplay *d) {
  char ai[] = "computer player";

  rasonly(d);
  glColor3f(1.0, 1.0, 1.0);
  drawText(gameFtx, d->vp_w / 4, 10, d->vp_w / (2 * strlen(ai)), ai);
  /* glRasterPos2i(100, 0); */
}

void drawPause(gDisplay *display) {
  char pause[] = "Game is paused";
  char winner[] = "Player %d wins!";
  char nowinner[] = "No one wins!";
  char buf[100];
  char *message;
  static float d = 0;
  static float lt = 0;
  float delta;
  long now;

  now = SystemGetElapsedTime();
  delta = now - lt;
  lt = now;
  delta /= 500.0;
  d += delta;
  /* printf("%.5f\n", delta); */
  
  if(d > 2 * M_PI) { 
    d -= 2 * M_PI;
  }

  if((game->pauseflag & PAUSE_GAME_FINISHED) &&
     game->winner != -1) {
    if(game->winner >= -1) {
    message = buf;
    sprintf(message, winner, game->winner + 1);
    } else {
      message = nowinner;
    }
  } else {
    message = pause;
  }

  rasonly(game->screen);
  glColor3f(1.0, (sin(d) + 1) / 2, (sin(d) + 1) / 2);
  drawText(gameFtx, display->vp_w / 6, 20, 
	   display->vp_w / (6.0 / 4.0 * strlen(message)), message);
}

void initGLGame() {
  glShadeModel( game->screen->shademodel );
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}
