#include "gltron.h"

#define TEX_SPLIT (1.0 - BOW_DIST2) / (1 - BOW_DIST1)
#undef TEX_SPLIT

static float normal1[] = { 1.0, 0.0, 0.0 };
static float normal2[] = { 0.0, 1.0, 0.0 };

/* 
   getDists returns the minimum distance from (the wall) *line to the
   specified (eye) point
   the z component is ignored
 */
float getDist(Line *ln, float* eye) {

  float n[2];
  float tmp[2];
  n[0] = ln->sx + (ln->ey - ln->sy);
  n[1] = ln->sy - (ln->ex - ln->sx);
  tmp[0] = eye[0] - ln->sx;
  tmp[1] = eye[1] - ln->sy;
  if(n[0] == n[1] == 0) return length(tmp);
  return abs(scalarprod2(n, tmp) / length(n));
}

/*
  getSegmentEnd[XY]() returns the end point of the
  last trail segment line (before the lightcycles bow starts)
*/

float dists[] = { BOW_DIST2, BOW_DIST3, BOW_DIST1, 0 };

float getSegmentEndX(Data *data, int dist) {
  float tlength, blength;
	Line *line = data->trails + data->trailOffset;
	
  if(dirsX[data->dir] == 0) return data->posx;

  tlength = data->posx - line->sx + data->posy - line->sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return data->posx - dists[dist] * blength * dirsX[ data->dir ];
}

float getSegmentEndY(Data *data, int dist) {
  float tlength, blength;
	Line *line = data->trails + data->trailOffset;

  if(dirsY[data->dir] == 0) return data->posy;

  tlength = data->posx - line->sx + data->posy - line->sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return data->posy - dists[dist] * blength * dirsY[ data->dir ];
}

/* getSegmentEndUV() calculates the texture coordinates for the last segment */
float getSegmentEndUV(Line *line, Data *data) {
  float tlength, blength;
  tlength = data->posx - line->sx + data->posy - line->sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return (tlength - 2 * blength) / DECAL_WIDTH;
}

/* getSegmentUV gets UV coordinates for an ordinary segment */
float getSegmentUV(Line *line) {
  float tlength;
  tlength = line->ex - line->sx + line->ey - line->sy;
  if(tlength < 0) tlength = -tlength;
  return tlength / DECAL_WIDTH;
}

/* 
   drawTrailLines() draws a white line on top of each trail segment
   the alpha value is reduced with increasing distance to the player
*/

void drawTrailLines(Player *p) {
  Line *line;
  float height;

  float *normal;
  float dist;
  float alpha;
  Data *data;
  Camera *cam;

  float trail_top[] = { 1.0, 1.0, 1.0, 1.0 };

  data = p->data;
  cam = p->camera;

  height = data->trail_height;
  if(height <= 0) return;

  /*
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  */

  if (game2->settingsCache.antialias_lines) {
    glEnable(GL_LINE_SMOOTH); /* enable line antialiasing */
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);

  glBegin(GL_LINES);

  line = &(data->trails[0]);
  while(line != data->trails + data->trailOffset) { 
		/* the current line is not drawn */
    /* compute distance from line to eye point */
    dist = getDist(line, cam->cam);
		alpha = (game2->rules.grid_size - dist / 2) / game2->rules.grid_size;
    // trail_top[3] = alpha;
    glColor4fv(trail_top);
    
    if(line->sy == line->ey) normal = normal1;
    else normal = normal2;
    glNormal3fv(normal);
    glVertex3f(line->sx, line->sy, height);
    glVertex3f(line->ex, line->ey, height);

    line++;
    polycount++;
  }
  glEnd();

  /* compute distance from line to eye point */
  dist = getDist(line, cam->cam);
  alpha = (game2->rules.grid_size - dist / 2) / game2->rules.grid_size;
	// trail_top[3] = alpha;
  glColor4fv(trail_top);

  glBegin(GL_LINES);

  glVertex3f(line->sx, line->sy, height);
  glVertex3f( getSegmentEndX(data, 0),
	      getSegmentEndY(data, 0),
	      height );

  glEnd();

  /* glEnable(GL_LIGHTING); */
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH); /* disable line antialiasing */
  
  /*
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  */
}

/* 
   drawTrailShadow() draws a alpha-blended shadow on the floor for each
   trail segment.
   The light source source is (in homogenous coordinates)
   at (-1,-1,1,0) (I hope that's correct)
*/

void drawTrailShadow(Player* p) {
  Line *line;
  float height;
  float ex, ey;
  Data *data;
  data = p->data;

  /* states */

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

  /* transformation */

  glPushMatrix();
  glMultMatrixf(shadow_matrix);

  height = data->trail_height;

  /* render */

  glBegin(GL_QUADS);
  line = &(data->trails[0]);
  while(line != data->trails + data->trailOffset) 
		{ /* the current line is not drawn */
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(line->sx, line->sy, 0);
    glVertex3f(line->sx, line->sy, height);
    glVertex3f(line->ex, line->ey, height);
    glVertex3f(line->ex, line->ey, 0);
    line++;
    polycount++;
  }
  glVertex3f(line->sx, line->sy, 0);
  glVertex3f(line->sx, line->sy, height);
  ex = getSegmentEndX(data, 1);
  ey = getSegmentEndY(data, 1);
  glVertex3f(ex, ey, height);
  glVertex3f(ex, ey, 0.0);
  glVertex3f(ex, ey, 0.0);
  glVertex3f(ex, ey, height);
  ex = getSegmentEndX(data, 0);
  ey = getSegmentEndY(data, 0);
  glVertex3f(ex, ey, height);
  glVertex3f(ex, ey, 0.0);
  glEnd();

  /* trail bow */
  drawTrailBow(p);

  /* restore */

  if(game2->settingsCache.use_stencil)
    glDisable(GL_STENCIL_TEST);

  glDisable(GL_BLEND);

  glPopMatrix();
}

void drawTrailBow(Player *p) {
  Data *data;
  float height;
  float ex, ey, sx, sy;
  int bdist;

  data = p->data;
  height = data->trail_height;
  if(height < 0) return;

  if(PLAYER_IS_ACTIVE(p)) {
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_TRAIL]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }


  bdist = PLAYER_IS_ACTIVE(p) ? 2 : 3;

  sx = getSegmentEndX(data, 0);
  sy = getSegmentEndY(data, 0);

  ex = getSegmentEndX(data, bdist);
  ey = getSegmentEndY(data, bdist);

    /* quad fading from white to model color, bow texture */
  glBegin(GL_QUADS);

  /* glTexCoord2f(TEX_SPLIT, 0.0); */
  glTexCoord2f(0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
  glVertex3f(sx, sy, 0.0);

  glTexCoord2f(1.0, 0.0);
	glColor3fv(p->pColorDiffuse);
  glVertex3f(ex, ey, 0.0);

  glTexCoord2f(1.0, 1.0);
	glColor3fv(p->pColorDiffuse);
  glVertex3f(ex, ey, height);

  /* glTexCoord2f(TEX_SPLIT, 1.0); */
  glTexCoord2f(0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
  glVertex3f(sx, sy, height);
  glEnd();

  polycount += 4;

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}
