#include "gltron.h"
#include "geom.h"

void drawDebugTex(gDisplay *d) {
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
  diff = (dt > 0) ? dt : 1;

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

void drawTraces(Player *p, gDisplay *d, int instance) {
  line *line;
  float height;
  float bdist;
  float blength;
  float tlength;

  Data *data;

  data = p->data;
  height = data->trail_height;

  if(height < 0) return;

  if(game->settings->alpha_trails) {
    glColor4fv(p->model->color_alpha);
    glDepthMask(GL_FALSE);
  } else {
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, game->screen->texTrailDecal);

    glColor3fv(p->model->color_alpha);
  }

  line = &(data->trails[0]);

#define DECAL_WIDTH 20.0
#define BOW_LENGTH 6

#define BOW_DIST2 0.85
#define BOW_DIST1 0.4

#define TEX_SPLIT (1.0 - BOW_DIST2) / (1 - BOW_DIST1)

  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(line->sx, line->sy, 0.0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(line->sx, line->sy, height);

  while(line != data->trail) {
    tlength = (line->ex - line->sx + line->ey - line->sy);
    if(tlength < 0) tlength = -tlength;

    glTexCoord2f(tlength / DECAL_WIDTH, 1.0);
    glVertex3f(line->ex, line->ey, height);
    glTexCoord2f(tlength / DECAL_WIDTH, 0.0);
    glVertex3f(line->ex, line->ey, 0.0);

    line++;

    glTexCoord2f(0.0, 0.0);
    glVertex3f(line->sx, line->sy, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(line->sx, line->sy, height);

    polycount++;
  }

  /* calculate distance of cycle to last corner */
  tlength = (line->ex - line->sx + line->ey - line->sy);
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;


  /* modify end of trail */
  glTexCoord2f((tlength - 2 * blength) / DECAL_WIDTH, 1.0);
  glVertex3f(line->ex - 2 * blength * dirsX[ data->dir ], 
	     line->ey - 2 * blength * dirsY[ data->dir ], height);
  glTexCoord2f((tlength - 2 * blength) / DECAL_WIDTH, 0.0);
  glVertex3f(line->ex - 2 * blength * dirsX[ data->dir ], 
	     line->ey - 2 * blength * dirsY[ data->dir ], 0.0);

  polycount += 2;
  glEnd();

  glDisable(GL_TEXTURE_2D);

    /* experimental trail effect */
  checkGLError("before trail");

  bdist = (game->settings->show_model &&
	   data->speed > 0) ? BOW_DIST1 : 0;

  /* quad fading from model color to white, no texture */
  glShadeModel(GL_SMOOTH);

  glBegin(GL_QUADS);

  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(line->ex - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     line->ey - BOW_DIST2 * blength * dirsY[ data->dir ], 0.0);

  glVertex3f(line->ex - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     line->ey - BOW_DIST2 * blength * dirsY[ data->dir ], height);


  if(game->settings->alpha_trails) glColor4fv(p->model->color_alpha);
  else glColor3fv(p->model->color_alpha);

  glVertex3f(line->ex - 2 * blength * dirsX[ data->dir ], 
	     line->ey - 2 * blength * dirsY[ data->dir ], height);


  glVertex3f(line->ex - 2 * blength * dirsX[ data->dir ], 
	     line->ey - 2 * blength * dirsY[ data->dir ], 0.0);

  glEnd();


  if(data->speed > 0 && game->settings->show_model == 1) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, game->screen->texTrail);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }


    /* quad fading from white to model color, bow texture */
  glBegin(GL_QUADS);

  glTexCoord2f(TEX_SPLIT, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(line->ex - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     line->ey - BOW_DIST2 * blength * dirsY[ data->dir ], 0.0);

  glTexCoord2f(1.0, 0.0);
  glColor3fv(p->model->color_model);
  glVertex3f(line->ex - bdist * blength * dirsX[ data->dir ], 
	     line->ey - bdist * blength * dirsY[ data->dir ], 0.0);

  glTexCoord2f(1.0, 1.0);
  glColor3fv(p->model->color_model);
  glVertex3f(line->ex - bdist * blength * dirsX[ data->dir ], 
	     line->ey - bdist * blength * dirsY[ data->dir ], height);

  glTexCoord2f(TEX_SPLIT, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(line->ex - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     line->ey - BOW_DIST2 * blength * dirsY[ data->dir ], height);
  glEnd();

  polycount += 4;

#undef BOW_DIST1
#undef BOW_DIST2
#undef TEX_SPLIT

#undef DECAL_WIDTH
#undef BOW_LENGTH

  glShadeModel( game->screen->shademodel );
  glDisable(GL_TEXTURE_2D);

  checkGLError("after trail");

    /* draw this line if in behind camera mode */

  if(game->settings->camType == 1) {
    /*       glLineWidth(3); */
    /* glBegin(GL_LINES); */
    glBegin(GL_QUADS);
#define LINE_D 0.05
    glVertex2f(data->trail->sx - LINE_D, data->trail->sy - LINE_D);
    glVertex2f(data->trail->sx + LINE_D, data->trail->sy + LINE_D);
    glVertex2f(data->trail->ex + LINE_D, data->trail->ey + LINE_D);
    glVertex2f(data->trail->ex - LINE_D, data->trail->ey - LINE_D);
    glEnd();
    /* glLineWidth(1); */
    polycount += 2;
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawCrash(float radius) {
#define CRASH_W 32
#define CRASH_H 16

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

#define turn_length 200

  dirangles = dirangles2;
  neigung_dir = -1.0;

  if(game->settings->model_backwards) {
     dirangles = dirangles1;
     neigung_dir = 1.0;
  }

  cycle = p->model->mesh[lod];
    
  glPushMatrix();
  glTranslatef(p->data->posx, p->data->posy, .0);

  if(game->settings->turn_cycle) {
    time = abs(p->data->turn_time - SystemGetElapsedTime());
    if(time < turn_length) {
      last_dir = p->data->last_dir;
      if(p->data->dir == 3 && last_dir == 2)
	last_dir = 4;
      if(p->data->dir == 2 && last_dir == 3)
	last_dir = 5;
      dirangle = ((turn_length - time) * dirangles[last_dir] +
		  time * dirangles[p->data->dir]) / turn_length;
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
    if(time < turn_length) {
      float axis = 1.0;
      if(p->data->dir < p->data->last_dir && p->data->last_dir != 3)
	axis = -1.0;
      else if((p->data->last_dir == 3 && p->data->dir == 2) ||
	      (p->data->last_dir == 0 && p->data->dir == 3))
	axis = -1.0;
      glRotatef(neigung * sin(M_PI * time / turn_length),
		0.0, axis * neigung_dir, 0.0);
    }
  }

  glTranslatef(-cycle->bbox[0] / 2, -cycle->bbox[1] / 2, .0);
  /* glTranslatef(-cycle->bbox[0] / 2, 0, .0); */
  /* glTranslatef(-cycle->bbox[0] / 2, -cycle->bbox[1], .0); */

  glEnable(GL_LIGHTING);
  /* glEnable(GL_CULL_FACE); */

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

  if(game->settings->show_alpha == 0) glDisable(GL_BLEND);

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
  glLightfv(GL_LIGHT0, GL_POSITION, p->camera->cam);

  gluLookAt(p->camera->cam[0], p->camera->cam[1], p->camera->cam[2],
	    p->camera->target[0], p->camera->target[1], p->camera->target[2],
	    0, 0, 1);

  drawFloor(d);
  if(game->settings->show_wall == 1)
    drawWalls(d);

  for(i = 0; i < game->players; i++)
    drawTraces(&(game->player[i]), d, i);

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

void initCustomLights() {
  /* float col[] = { .77, .77, .77, 1.0 }; */
  float col[] = { .95, .95, .95, 1.0 };
  float dif[] =  { 0.4, 0.4, 0.4, 1};
  float amb[] = { 0.05, 0.05, 0.05, 1};

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
  glLightfv(GL_LIGHT0, GL_SPECULAR, col);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
}

void initGLGame() {
  printf("OpenGL Info: '%s'\n%s - %s\n", glGetString(GL_VENDOR),
	 glGetString(GL_RENDERER), glGetString(GL_VERSION));
  printf("Extensions available: %s\n", glGetString(GL_EXTENSIONS));

  glShadeModel( game->screen->shademodel );


  if(game->settings->show_alpha) 
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* 
  glFogf(GL_FOG_START, 50.0);
  glFogf(GL_FOG_END, 100.0);
  glFogf(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY, 0.1);
  glDisable(GL_FOG);
  */


  /* TODO(3): incorporate model stuff */
  /* initLightAndMaterial(); */
  initCustomLights();

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}



