#include "gltron.h"

static float alpha = 0;
const static float rec_scale_factor = 0.25;

static float xv[] = { 0.5, 0.3245, 0.6, 0.5, 0.68, - 0.3 };
static float yv[] = { 0.8, 1.0, 0.0, 0.2, 0.2, 0.0 };

static float x() { return xv[0] * sin(xv[1] * alpha + xv[2]) - xv[3] * sin(xv[4] * alpha + xv[5]); }
static float y() { return yv[0] * cos(yv[1] * alpha + yv[2]) - yv[3] * sin(yv[4] * alpha + yv[5]); }

static float dx() { return xv[1] * xv[0] * cos(xv[1] * alpha + xv[2]) - xv[4] * xv[3] * cos(xv[4] * alpha + xv[5]); }
static float dy() { return - yv[1] * yv[0] * sin(yv[1] * alpha + yv[2]) - yv[4] * yv[3] * sin(yv[4] * alpha + yv[5]); }

void getRecognizerPositionVelocity(Point *p, Point *v) {
  p->x = ( x() + 1.0 ) * game2->rules.grid_size / 2.0;
  p->y = ( y() + 1.0 ) * game2->rules.grid_size / 2.0;
  v->x = dx() * game2->rules.grid_size;
  v->y = dy() * game2->rules.grid_size;
}

void drawRecognizers(int flag) {
  float phi, rx, ry;
  float max = 0;
  
  phi = acos ( dx() / sqrt( dx() * dx() + dy() * dy() ) );
  if( dy() < 0 )
    phi = 2 * M_PI - phi;

  glPushMatrix();

#if 0
  for(i = 0; i < 3; i++)
    if(recognizer->BBox.v[i] > max)
      max = recognizer->BBox.v[i];
#endif

  /* 
   * TODO: precompute and store max since it doesn't change from frame to frame 
   */
  max = recognizer->BBox.vSize.v[0] * rec_scale_factor;

  rx = ( max + ( x() + 1.0 ) * (game2->rules.grid_size - max) ) / 2.0;
  ry = ( max + ( y() + 1.0 ) * (game2->rules.grid_size - max) ) / 2.0;
  glTranslatef( rx, ry, RECOGNIZER_HEIGHT);
  glRotatef((phi + M_PI / 2) * 180 / M_PI, 0, 0, 1); /* up */

  glScalef(rec_scale_factor, rec_scale_factor, rec_scale_factor);

  if (flag) {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glLightfv(GL_LIGHT2, GL_SPECULAR, rec_spec_color); 
    glEnable(GL_LIGHT2);
 
    glDisable(GL_BLEND);
    if (getSettingi("light_cycles"))
      glEnable(GL_LIGHTING);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1,1);
  }

  glEnable(GL_NORMALIZE);
  glColor3f(0.0, 0.0, 0.0);
  drawModel(recognizer);
 
  if (flag) {
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LIGHT2);
    glEnable(GL_LIGHT1);
    glDisable(GL_LIGHTING);

    glColor3fv(rec_outline_color);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    drawQuadModel(recognizer_quad);
    glDisable(GL_LINE_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  glPopMatrix();
}  

void recognizerMovement() {
  alpha += game2->time.dt / 2000.0;
}

