#include "gltron.h"

static float alpha = 0;

static float xv[] = { 0.5, 0.3245, 0.6, 0.5, 0.68, - 0.3 };
static float yv[] = { 0.8, 1.0, 0.0, 0.2, 0.2, 0.0 };

static float x() { return xv[0] * sin(xv[1] * alpha + xv[2]) - xv[3] * sin(xv[4] * alpha + xv[5]); }
static float y() { return yv[0] * cos(yv[1] * alpha + yv[2]) - yv[3] * sin(yv[4] * alpha + yv[5]); }

static float dx() { return xv[1] * xv[0] * cos(xv[1] * alpha + xv[2]) - xv[4] * xv[3] * cos(xv[4] * alpha + xv[5]); }
static float dy() { return - yv[1] * yv[0] * sin(yv[1] * alpha + yv[2]) - yv[4] * yv[3] * sin(yv[4] * alpha + yv[5]); }

void getRecognizerPositionVelocity(Point *p, Point *v) {
  p->x = ( x() + 1.0 ) * game->settings->grid_size / 2.0;
  p->y = ( y() + 1.0 ) * game->settings->grid_size / 2.0;
  v->x = dx() * game->settings->grid_size;
  v->y = dy() * game->settings->grid_size;
}

void drawRecognizers(int flag) {
  float phi, rx, ry;
  float max = 0;
  int i;

  phi = acos ( dx() / sqrt( dx() * dx() + dy() * dy() ) );
  // printf("x: %.2f y: %.2f phi: %.2f ", dx(), dy(), phi);
  if( dy() < 0 )
    phi = 2 * M_PI - phi;
  // printf("phi corrected: %.2f\n", phi);
  // printf("recognizer in degrees: %.2f\n", (phi + M_PI / 2) * 180 / M_PI);
  glPushMatrix();

  for(i = 0; i < 3; i++)
    if(recognizer->bbox[i] > max)
      max = recognizer->bbox[i];

  rx = ( max + ( x() + 1.0 ) * (game->settings->grid_size - max) ) / 2.0;
  ry = ( max + ( y() + 1.0 ) * (game->settings->grid_size - max) ) / 2.0;
  glTranslatef( rx, ry, 20 );
  glRotatef( (phi + M_PI / 2) * 180 / M_PI, 0, 0, 1); /* up */

  if(flag) { 
    initModelLights(GL_LIGHT1);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);

    glDisable(GL_BLEND);
    if(game->settings->light_cycles)
      glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.4, 0.8, 0.0);
  }
    drawModel(recognizer, MODEL_FLAT, MODEL_SOLID);  
  // glColor3f(1, 1, 1);

  // drawModel(recognizer, MODEL_USE_MATERIAL, MODEL_SOLID);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);

  glPopMatrix();
}

void recognizerMovement() {
  alpha += game2->time.dt / 2000.0;
}









