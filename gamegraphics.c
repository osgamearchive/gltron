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
  polycount++;
}

void drawScore(Player *p, gDisplay *d) {
  char tmp[10]; /* hey, they won't reach such a score */

  sprintf(tmp, "%d", p->data->score);
  rasonly(d);
  glColor4f(1.0, 1.0, 0.2, 1.0);
  drawText(5, 5, 32, tmp);
}
  
void drawFloor(gDisplay *d) {
  int j, k, l, t;

  if(game->settings->show_floor_texture) {
    glDepthMask(GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, game->screen->texFloor);
    /* there are some strange clipping artefacts in software mode */
    /* try subdividing things... */
    glColor4f(1.0, 1.0, 1.0, 1.0);
    l = game->settings->grid_size / 4;
    t = 5;
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
	polycount++;
      }
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);
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
}

void drawTraces(Player *p, gDisplay *d, int instance) {
  line *line;
  float height;

  Data *data;
  data = p->data;
  height = data->trail_height;
  if(height > 0) {
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glColor4fv(p->model->color_alpha);
    /* glColor4f(0.5, 0.5, 0.5, 0.8); */
    line = &(data->trails[0]);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(line->sx, line->sy, 0.0);
    glVertex3f(line->sx, line->sy, height);
    while(line != data->trail) {
      glVertex3f(line->ex, line->ey, 0.0);
      glVertex3f(line->ex, line->ey, height);    
      line++;
      polycount++;
    }
    glVertex3f(line->ex, line->ey, 0.0);
    glVertex3f(line->ex, line->ey, height);
    polycount += 2;
    glEnd();

    if(game->settings->camType == 1) {
      //       glLineWidth(3);
      // glBegin(GL_LINES);
      glBegin(GL_QUADS);
#define LINE_D 0.05
      glVertex2f(data->trail->sx - LINE_D, data->trail->sy - LINE_D);
      glVertex2f(data->trail->sx + LINE_D, data->trail->sy + LINE_D);
      glVertex2f(data->trail->ex + LINE_D, data->trail->ey + LINE_D);
      glVertex2f(data->trail->ex - LINE_D, data->trail->ey - LINE_D);

      glEnd();
      // glLineWidth(1);
      polycount++;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

void drawCrash(float radius) {
#define CRASH_W 32
#define CRASH_H 16

  glColor4f(1.0, 1.0, 1.0, (EXP_RADIUS_MAX - radius) / EXP_RADIUS_MAX);
  /* printf("exp_r: %.2f\n", (EXP_RADIUS_MAX - radius) / EXP_RADIUS_MAX); */
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, game->screen->texCrash);
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
  glDisable(GL_TEXTURE_2D);
  if(game->settings->show_alpha == 0) glDisable(GL_BLEND);
}

void drawCycle(Player *p, int lod) {
  float dirangles1[] = { 180, 90, 0, 270, 360, -90 };
  float dirangles2[] = { 0, -90, -180, 90, 180, -270 };
  float *dirangles;
  int neigung_dir;
  int time;
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
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);

  if(p->data->exp_radius == 0)
    drawModel(cycle, MODEL_USE_MATERIAL, 0);
  else if(p->data->exp_radius < EXP_RADIUS_MAX) {
    float alpha;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    alpha = (float) (EXP_RADIUS_MAX - p->data->exp_radius) /
      (float) EXP_RADIUS_MAX;
    setMaterialAlphas(cycle, alpha);
    drawExplosion(cycle, p->data->exp_radius, MODEL_USE_MATERIAL, 0);
  }

  if(game->settings->show_alpha == 0) glDisable(GL_BLEND);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_FALSE);

  glPopMatrix();
}

int playerVisible(Player *eye, Player *target) {
  float v1[3];
  float v2[3];
  float tmp[3];
  float s;
  float d;
  int i;

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
    /* calculate lod */
    vsub(eye->camera->cam, tmp, v1);
    d = length(v1);
    for(i = 0; i < target->model->lod; i++) {
      if(d < target->model->lod_dist[i])
	return i;
    }
    return -1;
  }
}
	    
void drawPlayers(Player *p) {
  int i;
  int dir;
  float l = 5.0;
  float height;
  int lod;

  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  for(i = 0; i < game->players; i++) {
    height = game->player[i].data->trail_height;
    if(height > 0) {
      glPushMatrix();
      glTranslatef(game->player[i].data->posx,
		   game->player[i].data->posy,
		   0);
      /* draw Quad */
      dir = game->player[i].data->dir;
      glColor3fv(game->player[i].model->color_model);
      glBegin(GL_QUADS);
      glVertex3f(0, 0, 0);
      glColor4f(0, 0, 0, 0);
      glVertex3f(-dirsX[ dir ] * l, -dirsY[ dir ] * l, 0);
      glVertex3f(-dirsX[ dir ] * l, -dirsY[ dir ] * l, height);
      glColor3fv(game->player[i].model->color_model);
      glVertex3f(0, 0, height);
      glEnd();
      polycount++;
      glPopMatrix();
    }
    if(game->settings->show_model) {
      lod = playerVisible(p, &(game->player[i]));
      if(lod >= 0) 
	drawCycle(&(game->player[i]), lod);
    }
  }
  if(game->settings->show_alpha != 1) glDisable(GL_BLEND);
  glShadeModel(GL_FLAT);
}

void drawGlow(Player *p, gDisplay *d, float dim) {
  float mat[4*4];
  
  glPushMatrix();
  glTranslatef(p->data->posx,
               p->data->posy,
               0);
  /* draw Model */

  glShadeModel(GL_SMOOTH);
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


  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if(game->settings->show_alpha != 1) glDisable(GL_BLEND);
  glShadeModel(GL_FLAT);
  glPopMatrix();  
}

void drawWalls(gDisplay *d) {
#undef WALL_H
#define WALL_H 24
  float t = 2;
  glColor4f(1.0, 1.0, 1.0, 1.0);

  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  /* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */

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
  polycount += 4;

  glDisable(GL_TEXTURE_2D);

  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*
void drawHelp(gDisplay *d) {
  rasonly(d);
  glColor4f(0.2, 0.2, 0.2, 0.8);
  glEnable(GL_BLEND);
  glBegin(GL_QUADS);
  glVertex2i(0,0);
  glVertex2i(d->vp_w - 1, 0);
  glVertex2i(d->vp_w - 1, d->vp_h - 1);
  glVertex2i(0, d->vp_h - 1);
  glEnd();
  if(game->settings->show_alpha != 1) glDisable(GL_BLEND);
  glColor3f(1.0, 1.0, 0.0);
  drawLines(d->vp_w, d->vp_h,
	    help, HELP_LINES, 0);
}
*/

void drawCam(Player *p, gDisplay *d) {
  int i;

  if (d->fog == 1) glEnable(GL_FOG);

  glColor3f(0.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(game->settings->fov, d->vp_w / d->vp_h,
		 3.0, game->settings->grid_size * 1.5);

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

  glDisable(GL_FOG);
}

void drawAI(gDisplay *d) {
  char ai[] = "computer player";

  rasonly(d);
  glColor3f(1.0, 1.0, 1.0);
  drawText(d->vp_w / 4, 10, d->vp_w / (2 * strlen(ai)), ai);
  /* glRasterPos2i(100, 0); */
}

void drawPause(gDisplay *display) {
  char pause[] = "Game is paused";
  char winner[] = "Player %d wins";
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

  if(game->pauseflag & PAUSE_GAME_FINISHED &&
     game->winner != -1) {
    message = buf;
    sprintf(message, winner, game->winner + 1);
  } else {
    message = pause;
  }

  rasonly(game->screen);
  glColor3f(1.0, (sin(d) + 1) / 2, (sin(d) + 1) / 2);
  drawText(display->vp_w / 6, 20, 
	   display->vp_w / (6.0 / 4.0 * strlen(message)), message);
}



