#include "gltron.h"

void drawTraces(Player *p, gDisplay *d) {
  line *line;
  float height;
  float bdist;
  float blength;
  float tlength;

  float px, py;
  float sx, sy;

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

  while(line != data->trail) { /* the current line is not drawn */
    tlength = (line->ex - line->sx + line->ey - line->sy);
    if(tlength < 0) tlength = -tlength;
    
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
  glShadeModel(GL_SMOOTH);

  glBegin(GL_QUADS);

  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], 0.0);

  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], height);

  if(game->settings->alpha_trails) glColor4fv(p->model->color_alpha);
  else glColor3fv(p->model->color_alpha);

  glVertex3f(px - 2 * blength * dirsX[ data->dir ], 
	     py - 2 * blength * dirsY[ data->dir ], height);


  glVertex3f(px - 2 * blength * dirsX[ data->dir ], 
	     py - 2 * blength * dirsY[ data->dir ], 0.0);

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

  glTexCoord2f(TEX_SPLIT, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(px - BOW_DIST2 * blength * dirsX[ data->dir ], 
	     py - BOW_DIST2 * blength * dirsY[ data->dir ], height);
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
    glColor3fv(p->model->color_alpha);
    glVertex2f(sx - LINE_D, sy - LINE_D);
    glVertex2f(sx + LINE_D, sy + LINE_D);
    glVertex2f(px + LINE_D, py + LINE_D);
    glVertex2f(px - LINE_D, py - LINE_D);
    glEnd();
    /* glLineWidth(1); */
    polycount += 2;
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_TRUE);
}
