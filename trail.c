#include "gltron.h"

#define DECAL_WIDTH 20.0
#define BOW_LENGTH 6

#define BOW_DIST2 0.85
#define BOW_DIST1 0.4

#define TEX_SPLIT (1.0 - BOW_DIST2) / (1 - BOW_DIST1)

void drawTrailShadow(Data* data) {
  line *line;
  float height;
  float blength;
  float tlength;

  float px, py;
  float sx, sy;

  /* draw trail shadow */
#define SHADOWH 0.0
  height = data->trail_height;
  if(game->settings->softwareRendering == 0) {
    glEnable(GL_POLYGON_OFFSET_FILL);

    line = &(data->trails[0]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.6);
    glBegin(GL_QUADS);

    line = &(data->trails[0]);
    while(line != data->trail) { /* the current line is not drawn */
      tlength = (line->ex - line->sx + line->ey - line->sy);
      if(tlength < 0) tlength = -tlength;
    
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(line->sx, line->sy, SHADOWH);
      glVertex3f(line->sx + height, line->sy + height, SHADOWH);
      glVertex3f(line->ex + height, line->ey + height, SHADOWH);
      glVertex3f(line->ex, line->ey, SHADOWH);
      line++;
      polycount++;
    }
    sx = line->sx;
    sy = line->sy;
    px = data->posx;
    py = data->posy;
    tlength = px - sx + py - sy;
    if(tlength < 0) tlength = -tlength;
    blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
    glVertex3f(line->sx, line->sy, SHADOWH);
    glVertex3f(line->sx + height, line->sy + height, SHADOWH);
    glVertex3f(px + height, 
	       py + height,
	       SHADOWH);
    glVertex3f(px, py, SHADOWH);
  
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
  }
}

void drawTraces(Player *p, gDisplay *d) {
  line *line;
  float height;
  float bdist;
  float blength;
  float tlength;

  float px, py;
  float sx, sy;
  float normal1[] = { 1.0, 0.0, 0.0 };
  float normal2[] = { 0.0, 1.0, 0.0 };
  float *normal;
  float color[4];

  Data *data;

  data = p->data;
  height = data->trail_height;

  if(height < 0) return;

  drawTrailShadow(data);
  /* draw trail now */
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
      glBindTexture(GL_TEXTURE_2D, game->screen->texTrailDecal);
    }
    setColor3fv(p->model->color_alpha);
  }

  glShadeModel(GL_SMOOTH);

  line = &(data->trails[0]);
  glBegin(GL_QUADS);
  while(line != data->trail) { /* the current line is not drawn */
    tlength = (line->ex - line->sx + line->ey - line->sy);
    if(tlength < 0) tlength = -tlength;
    
    if(line->sy == line->ey) normal = normal1;
    else normal = normal2;

    glNormal3fv(normal);
    setNormal3fv(normal);
    setVertex3f((line->sx + line->ex) / 2, (line->sy + line->ey) / 2, 0);
    light4fv(color);
    glColor4fv(color);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(line->sx, line->sy, 0.0);

    glTexCoord2f(0.0, 1.0);
    glVertex3f(line->sx, line->sy, height);

    glTexCoord2f(tlength / DECAL_WIDTH, 1.0);
    glVertex3f(line->ex, line->ey, height);

    glTexCoord2f(tlength / DECAL_WIDTH, 0.0);
    glVertex3f(line->ex, line->ey, 0.0);

    line++;
    polycount++;
  }


  sx = line->sx;
  sy = line->sy;
  px = data->posx;
  py = data->posy;

  if(sy == py) normal = normal1;
  else normal = normal2;
  glNormal3fv(normal);

  setNormal3fv(normal);
  setVertex3f((line->sx + line->ex) / 2, (line->sy + line->ey) / 2, 0);
  light4fv(color);
  glColor4fv(color);
    
  /* calculate distance of cycle to last corner */
  tlength = px - sx + py - sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;

  glTexCoord2f(0.0, 0.0);
  glVertex3f(sx, sy, 0.0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(sx, sy, height);

  /* modify end of trail */

  glTexCoord2f((tlength - 2 * blength) / DECAL_WIDTH, 1.0);
  glVertex3f(px - 2 * blength * dirsX[ data->dir ], 
	     py - 2 * blength * dirsY[ data->dir ], height);
  glTexCoord2f((tlength - 2 * blength) / DECAL_WIDTH, 0.0);
  glVertex3f(px - 2 * blength * dirsX[ data->dir ], 
	     py - 2 * blength * dirsY[ data->dir ], 0.0);

  polycount += 2;
  glEnd();

  glDisable(GL_TEXTURE_2D);

    /* experimental trail effect */
  checkGLError("before trail");

  bdist = (game->settings->show_model &&
	   data->speed > 0) ? BOW_DIST1 : 0;

  /* quad fading from model color to white, no texture */

  glBegin(GL_QUADS);

  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], 0.0);

  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], height);

  glColor4fv(color);


  glVertex3f(px - 2 * blength * dirsX[ data->dir ], 
	     py - 2 * blength * dirsY[ data->dir ], height);


  glVertex3f(px - 2 * blength * dirsX[ data->dir ], 
	     py - 2 * blength * dirsY[ data->dir ], 0.0);

  glEnd();

  glShadeModel( game->screen->shademodel );
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  /* draw lines on top of trails */
  glPolygonOffset(-2.0, -1.0);  
  glEnable(GL_POLYGON_OFFSET_LINE);


  line = &(data->trails[0]);
  glBegin(GL_LINES);
  glColor3f(1.0, 1.0, 1.0);
  while(line != data->trail) { /* the current line is not drawn */
    tlength = (line->ex - line->sx + line->ey - line->sy);
    if(tlength < 0) tlength = -tlength;
    
    if(line->sy == line->ey) normal = normal1;
    else normal = normal2;
    glNormal3fv(normal);
    glVertex3f(line->sx, line->sy, height);
    glVertex3f(line->ex, line->ey, height);
    line++;
    polycount++;
  }
  glVertex3f(line->sx, line->sy, height);
  sx = line->sx;
  sy = line->sy;
  px = data->posx;
  py = data->posy;
  tlength = px - sx + py - sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;

  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], height);
  glEnd();

  glDisable(GL_POLYGON_OFFSET_LINE);


  glDepthMask(GL_TRUE);
}

void drawTrailBow(Player *p) {
  Data *data;
  line *line;

  float height;
  float tlength;
  float blength;
  float bdist;
  float px, py, sx, sy;

  data = p->data;
  height = data->trail_height;
  if(height < 0) return;

  bdist = (game->settings->show_model &&
	   data->speed > 0) ? BOW_DIST1 : 0;

  line = data->trail;

  sx = line->sx;
  sy = line->sy;
  px = data->posx;
  py = data->posy;
  tlength = px - sx + py - sy;
  if(tlength < 0) tlength = -tlength;
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;

  glShadeModel(GL_SMOOTH);

  if(data->speed > 0 && game->settings->show_model == 1) {
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, game->screen->texTrail);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }

    /* quad fading from white to model color, bow texture */
  glBegin(GL_QUADS);

  /* glTexCoord2f(TEX_SPLIT, 0.0); */
  glTexCoord2f(0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], 0.0);

  glTexCoord2f(1.0, 0.0);
  glColor3fv(p->model->color_model);
  glVertex3f(px - bdist * blength * dirsX[ data->dir ], 
	     py - bdist * blength * dirsY[ data->dir ], 0.0);

  glTexCoord2f(1.0, 1.0);
  glColor3fv(p->model->color_model);
  glVertex3f(px - bdist * blength * dirsX[ data->dir ], 
	     py - bdist * blength * dirsY[ data->dir ], height);

  /* glTexCoord2f(TEX_SPLIT, 1.0); */
  glTexCoord2f(0.0, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], height);
  glEnd();

  polycount += 4;

  glShadeModel( game->screen->shademodel );
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

}

#undef SHADOWH

#undef BOW_DIST1
#undef BOW_DIST2
#undef TEX_SPLIT

#undef DECAL_WIDTH
#undef BOW_LENGTH


