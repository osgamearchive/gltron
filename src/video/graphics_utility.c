#include "video/video.h"
#include "game/game.h"

void checkGLError(const char *where) {
  int error;
  error = glGetError();
  if(error != GL_NO_ERROR)
    printf("[glError: %s] - %d\n", where, error);
}

void rasonly(Visual *d) {
  /* do rasterising only (in local display d) */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, (GLfloat) d->vp_w, 0.0f, (GLfloat) d->vp_h, 0.0f, 1.0f);
  checkGLError("rasonly");
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
}


void doPerspective(float fov, float ratio, float znear, float zfar) {
  float top;
  float left;

  top = tanf( fov * PI / 360.0 ) * znear;
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

void drawText(FontTex* ftx, int x, int y, int size, const char *text) {
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
}

