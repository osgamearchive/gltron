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
float getDist(line *line, float* eye) {

  float n[2];
  float tmp[2];
  n[0] = line->sx + (line->ey - line->sy);
  n[1] = line->sy - (line->ex - line->sx);
  tmp[0] = eye[0] - line->sx;
  tmp[1] = eye[1] - line->sy;
  if(n[0] == n[1] == 0) return length(tmp);
  return abs(scalarprod2(n, tmp) / length(n));
}

/*
  getSegmentEnd[XY]() returns the end point of the
  last trail segment line (before the lightcycles bow starts)
*/

#define DECAL_WIDTH 20.0
#define BOW_LENGTH 6

#define BOW_DIST3 2
#define BOW_DIST2 0.85
#define BOW_DIST1 0.4

float dists[] = { BOW_DIST2, BOW_DIST3, BOW_DIST1, 0 };

float getSegmentEndX(line *line, Data *data, int dist) {
  float tlength, blength;

  if(dirsX[data->dir] == 0) return data->posx;

  tlength = data->posx - line->sx + data->posy - line->sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return data->posx - dists[dist] * blength * dirsX[ data->dir ];
}

float getSegmentEndY(line *line, Data *data, int dist) {
  float tlength, blength;
  if(dirsY[data->dir] == 0) return data->posy;

  tlength = data->posx - line->sx + data->posy - line->sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return data->posy - dists[dist] * blength * dirsY[ data->dir ];
}

/* getSegmentEndUV() calculates the texture coordinates for the last segment */
float getSegmentEndUV(line *line, Data *data) {
  float tlength, blength;
  tlength = data->posx - line->sx + data->posy - line->sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return (tlength - 2 * blength) / DECAL_WIDTH;
}

/* getSegmentUV gets UV coordinates for an ordinary segment */
float getSegmentUV(line *line) {
  float tlength;
  tlength = line->ex - line->sx + line->ey - line->sy;
  if(tlength < 0) tlength = -tlength;
  return tlength / DECAL_WIDTH;
}

#undef BOW_DIST1
#undef BOW_DIST2


#undef DECAL_WIDTH
#undef BOW_LENGTH


/* 
   drawTrailLines() draws a white line on top of each trail segment
   the alpha value is reduced with increasing distance to the player
*/

void drawTrailLines(Player *p) {
  line *line;
  float height;

  float *normal;
  float dist;
  float alpha;
  Data *data;
  Camera *cam;

  data = p->data;
  cam = p->camera;

  height = data->trail_height;
  if(height < 0) return;

  glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* glDisable(GL_LIGHTING); */
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);

  line = &(data->trails[0]);
  while(line != data->trail) { /* the current line is not drawn */
    /* compute distance from line to eye point */
    dist = getDist(line, cam->cam);
    alpha = (game->settings->grid_size - dist / 2) / game->settings->grid_size;
    /* printf("dist: %.2f, alpha: %.2f\n", dist, alpha); */
    glColor4f(1.0, 1.0, 1.0, alpha);
    
    if(line->sy == line->ey) normal = normal1;
    else normal = normal2;
    glNormal3fv(normal);
    glVertex3f(line->sx, line->sy, height);
    glVertex3f(line->ex, line->ey, height);
    line++;
    polycount++;
  }
  glEnd();
  
  glColor3f(1.0, 1.0, 1.0);
  /* compute distance from line to eye point */
  dist = getDist(line, cam->cam);
  alpha = (game->settings->grid_size - dist / 2) / game->settings->grid_size;
  /* printf("dist: %.2f, alpha: %.2f\n", dist, alpha); */
  glColor4f(1.0, 1.0, 1.0, alpha);
  glBegin(GL_LINES);

  glVertex3f(line->sx, line->sy, height);
  glVertex3f( getSegmentEndX(line, data, 0),
	      getSegmentEndY(line, data, 0),
	      height );

  glEnd();

  /* glEnable(GL_LIGHTING); */
  glDisable(GL_BLEND);

  glEnable(GL_DEPTH_TEST);
}

/* 
   drawTrailShadow() draws a alpha-blended shadow on the floor for each
   trail segment.
   The light source source is (in homogenous coordinates)
   at (-1,-1,1,0) (I hope that's correct)
*/

#define SHADOWH 0.0
void drawTrailShadow(Data* data) {
  line *line;
  float height;
  /* draw trail shadow */

  height = data->trail_height;
  if(game->settings->softwareRendering == 0) {
    line = &(data->trails[0]);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.6);
    glBegin(GL_QUADS);

    line = &(data->trails[0]);
    while(line != data->trail) { /* the current line is not drawn */
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(line->sx, line->sy, SHADOWH);
      glVertex3f(line->sx + height, line->sy + height, SHADOWH);
      glVertex3f(line->ex + height, line->ey + height, SHADOWH);
      glVertex3f(line->ex, line->ey, SHADOWH);
      line++;
      polycount++;
    }
    glVertex3f(line->sx, line->sy, SHADOWH);
    glVertex3f(line->sx + height, line->sy + height, SHADOWH);
    glVertex3f(data->posx + height, 
	       data->posy + height,
	       SHADOWH);
    glVertex3f(data->posx, data->posy, SHADOWH);
    glEnd();
    glEnable(GL_DEPTH_TEST);
  }
}
#undef SHADOWH

/*
   drawTraces() draws all the trail segments.
   The last one is split in three parts:
   One ordinary part, one fading from trail color to white, and
   a bow fading from white to the models color (that one is drawn
   seperately in drawTrailBow() ).
*/

void drawTraces(Player *p, gDisplay *d) {
  line *line;
  float height;
  float uv, ex, ey;
  float normal1[] = { 1.0, 0.0, 0.0 };
  float normal2[] = { 0.0, 1.0, 0.0 };
  float *normal;
  float color[4];

  Data *data;

  data = p->data;
  height = data->trail_height;

  if(height < 0) return;

  /* calculate trail color and set blending modes */
  if(game->settings->alpha_trails) {
    glColor4fv(p->model->color_alpha);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setColor4fv(p->model->color_alpha);
  } else {
    if(game->settings->softwareRendering == 0) {
      glDisable(GL_BLEND);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_DECAL]);
    }
    setColor3fv(p->model->color_alpha);
  }

  glShadeModel(GL_SMOOTH);

  /* start drawing */
  line = &(data->trails[0]);
  glBegin(GL_QUADS);
  while(line != data->trail) { /* the last segment is special cased */

    if(line->sy == line->ey) normal = normal1;
    else normal = normal2;

    /* glNormal3fv(normal); */
    setNormal3fv(normal);
    setVertex3f((line->sx + line->ex) / 2, (line->sy + line->ey) / 2, 0);
    light4fv(color);
    glColor4fv(color);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(line->sx, line->sy, 0.0);

    glTexCoord2f(0.0, 1.0);
    glVertex3f(line->sx, line->sy, height);

    uv = getSegmentUV(line);
    glTexCoord2f(uv, 1.0);
    glVertex3f(line->ex, line->ey, height);

    glTexCoord2f(uv, 0.0);
    glVertex3f(line->ex, line->ey, 0.0);

    line++;
    polycount++;
  }

  if(line->sy == data->posy) normal = normal1;
  else normal = normal2;
  /* glNormal3fv(normal); */

  /* calculate segment color */
  setNormal3fv(normal);
  setVertex3f(line->sx, line->sy, 0);
  light4fv(color);
  glColor4fv(color);
    
  glTexCoord2f(0.0, 0.0);
  glVertex3f(line->sx, line->sy, 0.0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(line->sx, line->sy, height);

  /* modify end of trail */

  uv = getSegmentEndUV(line, data);
  ex = getSegmentEndX(line, data, 1);
  ey = getSegmentEndY(line, data, 1);

  glTexCoord2f(uv, 1.0);
  glVertex3f(ex, ey, height);
  glTexCoord2f(uv, 0.0);
  glVertex3f(ex, ey, 0.0);

  polycount += 2;
  glEnd();

  glDisable(GL_TEXTURE_2D);

  /* experimental trail effect */
  checkGLError("before trail");

  glBegin(GL_QUADS);

  
  /* glColor3f(1.0, 0.0, 1.0); */
  glColor4fv(color);

  glVertex3f(ex, ey, 0);
  glVertex3f(ex, ey, height);

  glColor3f(1.0, 1.0, 1.0);
  ex = getSegmentEndX(line, data, 0);
  ey = getSegmentEndY(line, data, 0);

  glVertex3f(ex, ey, height);
  glVertex3f(ex, ey, 0);

  glEnd();

  glShadeModel( game->screen->shademodel );
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glDepthMask(GL_TRUE);
}

void drawTrailBow(Player *p) {
  Data *data;
  float height;
  float ex, ey, sx, sy;
  int bdist;

  data = p->data;
  height = data->trail_height;
  if(height < 0) return;

  glShadeModel(GL_SMOOTH);

  if(data->speed > 0 && game->settings->show_model == 1) {
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_TRAIL]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }


  bdist = (game->settings->show_model &&
	   data->speed > 0) ? 2 : 3;

  sx = getSegmentEndX(data->trail, data, 0);
  sy = getSegmentEndY(data->trail, data, 0);

  ex = getSegmentEndX(data->trail, data, bdist);
  ey = getSegmentEndY(data->trail, data, bdist);

    /* quad fading from white to model color, bow texture */
  glBegin(GL_QUADS);

  /* glTexCoord2f(TEX_SPLIT, 0.0); */
  glTexCoord2f(0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(sx, sy, 0.0);

  glTexCoord2f(1.0, 0.0);
  glColor3fv(p->model->color_model);
  glVertex3f(ex, ey, 0.0);

  glTexCoord2f(1.0, 1.0);
  glColor3fv(p->model->color_model);
  glVertex3f(ex, ey, height);

  /* glTexCoord2f(TEX_SPLIT, 1.0); */
  glTexCoord2f(0.0, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(sx, sy, height);
  glEnd();

  polycount += 4;

  glShadeModel( game->screen->shademodel );
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

}



