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
      // printf("minimum FPS: %d - average FPS: %d\n", min, sum / FPS_HSIZE);
    }
  }

  sprintf(tmp, "average FPS: %d", fps_avg);
  glColor4f(1.0, 0.4, 0.2, 1.0);
  drawText(d->vp_w - 180, d->vp_h - 20, 10, tmp);
  sprintf(tmp, "minimum FPS: %d", fps_min);
  drawText(d->vp_w - 180, d->vp_h - 35, 10, tmp);
}

void drawText(int x, int y, int size, char *text) {
  /* int i; */

  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  /* txfBindFontTexture(txf); */

  glPushMatrix();

  glTranslatef(x, y, 0);
  glScalef(size, size, size);
  ftxRenderString(ftx, text, strlen(text));
  
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  polycount += strlen(text);
}

int hsv2rgb(float h, float s, float v, float *r, float *g, float *b) {
  float j, f, m, n, k;
  int i;

  if(s == 0) 
    if(h == -1)
      *r = *g = *b = v;
    else return 1;
  else {
    if(h >= 360) h = 0;
    i = h / 60; /* integer */
    j = h / 60;

    f = j - i;
    m = v * (1 - s);
    n = v * (1 - s * f);
    k = v * (1 - s * (1 - f));
    switch(i) {
    case 0: *r = v; *g = k; *b = m; break;
    case 1: *r = n; *g = v; *b = m; break;
    case 2: *r = m; *g = v; *b = k; break;
    case 3: *r = m; *g = n; *b = v; break;
    case 4: *r = k; *g = m; *b = v; break;
    case 5: *r = v; *g = m; *b = n; break;
    }
  }
  return 0;
}
 
void colorDisc() {
  int h;
  float r, g, b;
  int shade_model;


  glGetIntegerv(GL_SHADE_MODEL, &shade_model);

  glShadeModel(GL_SMOOTH);
  glBegin(GL_TRIANGLE_FAN);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(0, 0, 0);
  for(h = 0; h <= 360; h += 10) {
    hsv2rgb(h, 1, 1, &r, &g, &b);
    glColor3f(r, g, b);
    /* printf("%.2f %.2f %.2f\n", r, g, b); */
    glVertex3f(cos(h * 2 * M_PI / 360), sin(h * 2 * M_PI / 360), 0);
  }
  glEnd();

  glShadeModel(shade_model);
}











