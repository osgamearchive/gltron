#include "gltron.h"

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
  if(game->settings->softwareRendering) {
    drawSoftwareText(ftx, x, y, size, text);
  } else {
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(size, size, size);
    ftxRenderString(ftx, text, strlen(text));
  
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    polycount += 2 * strlen(text); /* quads are two triangles */
  }
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
  /* float col[] = { .77, .77, .77, 1.0 }; */
  float col[] = { .0, .0, .0, 1.0 };
  float dif[] =  { 0.8, 0.8, 0.8, 1};
  float amb[] = { 0.3, 0.3, 0.3, 1};

  glLightfv(light, GL_AMBIENT, amb);
  glLightfv(light, GL_SPECULAR, col);
  glLightfv(light, GL_DIFFUSE, dif);
}

void rebuildDebugTex() {
  int x, y;
  int px = -1, py = -1;
  float tx, ty;
  int color;
  unsigned char *source;
  /* printf("rebuilding texture: %d -> ", SystemGetElapsedTime()); */
  for(y = 0; y < game->settings->grid_size; y++) {
    ty = (float) y * DEBUG_TEX_H / game->settings->grid_size;
    for(x = 0; x < game->settings->grid_size; x++) {
      tx = (float) x * DEBUG_TEX_W / game->settings->grid_size;
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
  size = game->settings->grid_size;

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
    if(data->speed > 0) {
      glBegin(GL_LINES);
      glColor3fv(p->model->color_alpha);
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
  glBindTexture(GL_TEXTURE_2D, d->texDebug);
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
  glBitmap(colwidth * 8 , game->settings->grid_size, 0, 0, 0, 0, colmap);
  glBegin(GL_LINE_LOOP);
  glVertex2i(x - 1, y - 1);
  glVertex2i(x + colwidth * 8, y - 1);
  glVertex2i(x + colwidth * 8, y + game->settings->grid_size);
  glVertex2i(x - 1, y + game->settings->grid_size);
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
  int lines = 5;
  rasonly(d);
  glColor3f(1.0, 0.3, 0.3);
  if(game->screen->vp_h < 600) lines = 3;
  if(game->settings->softwareRendering) lines = 1;
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
  
void drawFloor(gDisplay *d) {
  int j, k, l, t;

  glShadeModel(GL_FLAT);

  if(game->settings->show_floor_texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, game->screen->texFloor);
    /* there are some strange clipping artefacts in software mode */
    /* try subdividing things... */
    glColor4f(1.0, 1.0, 1.0, 1.0);
    l = game->settings->grid_size / 4;
    t = l / 12;
    for(j = 0; j < game->settings->grid_size; j += l)
      for(k = 0; k < game->settings->grid_size; k += l) {
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2i(j, k);
	glTexCoord2i(t, 0);
	glVertex2i(j + l, k);
	glTexCoord2i(t, t);
	glVertex2i(j + l, k + l);
	glTexCoord2i(0, t);
	glVertex2i(j, k + l);
	glEnd();
	polycount += 2;
      }
    glDisable(GL_TEXTURE_2D);
  } else {
    /* lines as floor... */
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    for(j = 0; j <= game->settings->grid_size;
	j += game->settings->line_spacing) {
      glVertex3i(0, j, 0);
      glVertex3i(game->settings->grid_size, j, 0);
      glVertex3i(j, 0, 0);
      glVertex3i(j, game->settings->grid_size, 0);
      polycount += 2;
    }
    glEnd();
  }

  glShadeModel( game->screen->shademodel );
}

void drawCrash(float radius) {
#define CRASH_W 32
#define CRASH_H 16
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glColor4f(1.0, 1.0, 1.0, (EXP_RADIUS_MAX - radius) / EXP_RADIUS_MAX);
  /* printf("exp_r: %.2f\n", (EXP_RADIUS_MAX - radius) / EXP_RADIUS_MAX); */
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, game->screen->texCrash);
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

void drawCycle(Player *p, int lod) {
  float dirangles1[] = { 180, 90, 0, 270, 360, -90 };
  float dirangles2[] = { 0, -90, -180, 90, 180, -270 };
  float *dirangles;
  int neigung_dir;
  int time = 0;
  int last_dir;
  float dirangle;
  Mesh *cycle;

  dirangles = dirangles2;
  neigung_dir = -1.0;

  if(game->settings->model_backwards) {
     dirangles = dirangles1;
     neigung_dir = 1.0;
  }

  cycle = p->model->mesh[lod];
    
  glPushMatrix();
  glTranslatef(p->data->posx, p->data->posy, 0.0);

  if(game->settings->turn_cycle) {
    time = game2->time.current - p->data->turn_time;
    if(time < TURN_LENGTH) {
      last_dir = p->data->last_dir;
      if(p->data->dir == 3 && last_dir == 2)
	last_dir = 4;
      if(p->data->dir == 2 && last_dir == 3)
	last_dir = 5;
      dirangle = ((TURN_LENGTH - time) * dirangles[last_dir] +
		  time * dirangles[p->data->dir]) / TURN_LENGTH;
    } else
      dirangle = dirangles[p->data->dir];
  } else  dirangle = dirangles[p->data->dir];

  glRotatef(dirangle, 0.0, 0.0, 1.0);

  if(game->settings->show_crash_texture) {
    if(p->data->exp_radius > 0 && p->data->exp_radius < EXP_RADIUS_MAX) {
      glPushMatrix();
      glRotatef(dirangles[p->data->dir] - dirangle, 0.0, 0.0, 1.0);
      drawCrash(p->data->exp_radius);
      glPopMatrix();
    }
  }

#define neigung 25
  if(game->settings->turn_cycle) {
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
  }

  glTranslatef(-cycle->bbox[0] / 2, -cycle->bbox[1] / 2, .0);
  /* glTranslatef(-cycle->bbox[0] / 2, 0, .0); */
  /* glTranslatef(-cycle->bbox[0] / 2, -cycle->bbox[1], .0); */

  initModelLights(GL_LIGHT1);
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);



  if(p->data->exp_radius == 0)
    drawModel(cycle, MODEL_USE_MATERIAL, 0);
  else if(p->data->exp_radius < EXP_RADIUS_MAX) {
    float alpha;
    if(game->settings->show_alpha == 1) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      alpha = (float) (EXP_RADIUS_MAX - p->data->exp_radius) /
	(float) EXP_RADIUS_MAX;
      setMaterialAlphas(cycle, alpha);
    }
    drawExplosion(cycle, p->data->exp_radius, MODEL_USE_MATERIAL, 0);
  }
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

static int lod_dist[][4] = { 
  { 25, 50, 150, 0 },
  { 5, 30, 150, 0 },
  { 1, 5, 100, 0 }
};
static int max_lod = 2;
 
int playerVisible(Player *eye, Player *target) {
  float v1[3];
  float v2[3];
  float tmp[3];
  float s;
  float d;
  int i;
  int lod;

  vsub(eye->camera->target, eye->camera->cam, v1);
  normalize(v1);
  tmp[0] = target->data->posx;
  tmp[1] = target->data->posy;
  tmp[2] = 0;
  vsub(tmp, eye->camera->cam, v2);
  normalize(v2);
  s = scalarprod(v1, v2);
  /* maybe that's not exactly correct, but I didn't notice anything */
  d = cos((game->settings->fov / 2) * 2 * M_PI / 360.0);
  /*
  printf("v1: %.2f %.2f %.2f\nv2: %.2f %.2f %.2f\ns: %.2f d: %.2f\n\n",
	 v1[0], v1[1], v1[2], v2[0], v2[1], v2[2],
	 s, d);
  */
  if(s < d)
    return -1;
  else {
    lod = (game->settings->lod > max_lod) ? max_lod : game->settings->lod;
    /* calculate lod */
    vsub(eye->camera->cam, tmp, v1);
    d = length(v1);
    for(i = 0; i < target->model->lod; i++) {
      if(d < lod_dist[lod][i])
	return i;
    }
    return -1;
  }
}
	    
void drawPlayers(Player *p) {
  int i;
  float height;
  int lod;

  glShadeModel(GL_SMOOTH);
  for(i = 0; i < game->players; i++) {
    height = game->player[i].data->trail_height;

    if(game->settings->show_model) {
      lod = playerVisible(p, &(game->player[i]));
      if(lod >= 0) 
	drawCycle(&(game->player[i]), lod);
    }
  }
  glShadeModel( game->screen->shademodel );
}

void drawGlow(Player *p, gDisplay *d, float dim) {
  float mat[4*4];
  
  glPushMatrix();
  glTranslatef(p->data->posx,
               p->data->posy,
               0);

  glShadeModel(GL_SMOOTH);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);

  glGetFloatv(GL_MODELVIEW_MATRIX, mat);
  mat[0] = mat[5] = mat[10] = 1.0;
  mat[1] = mat[2] = 0.0;
  mat[4] = mat[6] = 0.0;
  mat[8] = mat[9] = 0.0;
  glLoadMatrixf(mat);
  glBegin(GL_TRIANGLE_FAN);
  glColor3fv(p->model->color_model);

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
  glColor3fv(p->model->color_model);
  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(0,-TRAIL_HEIGHT/4,0);
  glVertex3f(dim*cos(-0.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(-0.2*3.1415/5.0), 0);

  glColor3fv(p->model->color_model);
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

void drawWalls(gDisplay *d) {
#undef WALL_H
#define WALL_H 48
  float t;

  t = game->settings->grid_size / 240.0;
  glColor4f(1.0, 1.0, 1.0, 1.0);

  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
#define T_TOP 1.0
  glBindTexture(GL_TEXTURE_2D, game->screen->texWall_1);
  glBegin(GL_QUADS);
    glTexCoord2f(t, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(t, T_TOP); glVertex3f(0.0, 0.0, WALL_H);
    glTexCoord2f(0.0, T_TOP); glVertex3f(game->settings->grid_size, 0.0, WALL_H);
    glTexCoord2f(0.0, 0.0); glVertex3f(game->settings->grid_size, 0.0, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->texWall_2);
  glBegin(GL_QUADS);
    glTexCoord2f(t, 0.0); glVertex3f(game->settings->grid_size, 0.0, 0.0);
    glTexCoord2f(t, T_TOP); glVertex3f(game->settings->grid_size, 0.0, WALL_H);
    glTexCoord2f(0.0, T_TOP); 
    glVertex3f(game->settings->grid_size, game->settings->grid_size, WALL_H);
    glTexCoord2f(0.0, 0.0); 
    glVertex3f(game->settings->grid_size, game->settings->grid_size, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->texWall_3);
  glBegin (GL_QUADS);
  glTexCoord2f(t, 0.0); 
    glVertex3f(game->settings->grid_size, game->settings->grid_size, 0.0);
    glTexCoord2f(t, T_TOP); 
    glVertex3f(game->settings->grid_size, game->settings->grid_size, WALL_H);
    glTexCoord2f(0.0, T_TOP); glVertex3f(0.0, game->settings->grid_size, WALL_H);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, game->settings->grid_size, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->texWall_4);
  glBegin(GL_QUADS);
    glTexCoord2f(t, 0.0); glVertex3f(0.0, game->settings->grid_size, 0.0);
    glTexCoord2f(t, T_TOP); glVertex3f(0.0, game->settings->grid_size, WALL_H);
    glTexCoord2f(0.0, T_TOP); glVertex3f(0.0, 0.0, WALL_H);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0); 
    #undef T_TOP
  glEnd();
  polycount += 8;

  glDisable(GL_TEXTURE_2D);

  glDisable(GL_CULL_FACE);
}

void drawCam(Player *p, gDisplay *d) {
  int i;

  float arena[] = { 1.0, 1.0, 1, 0.0 };


  /* 
  if (d->fog == 1) {
    glEnable(GL_FOG);
    fprintf(stderr, "who enabled the fog?\n");
  }
  */

  glColor3f(0.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(game->settings->fov, d->vp_w / d->vp_h,
		 1.0, game->settings->grid_size * 1.5);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_POSITION, arena);
  glLightfv(GL_LIGHT1, GL_POSITION, p->camera->cam);
  gluLookAt(p->camera->cam[0], p->camera->cam[1], p->camera->cam[2],
	    p->camera->target[0], p->camera->target[1], p->camera->target[2],
	    0, 0, 1);

  drawFloor(d);
  if(game->settings->show_wall == 1)
    drawWalls(d);


  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  initTrailLights(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  for(i = 0; i < game->players; i++)
    drawTraces(&(game->player[i]), d);
  glDisable(GL_LIGHTING);

  drawPlayers(p);

  /* draw the glow around the other players: */
  if(game->settings->show_glow == 1)
    for(i = 0; i < game->players; i++)
      if ((p != &(game->player[i])) && (game->player[i].data->speed > 0))
	drawGlow(&(game->player[i]), d, TRAIL_HEIGHT * 4);
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
    message = buf;
    sprintf(message, winner, game->winner + 1);
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
