#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sgi_texture.h"
#include "gltron.h"

sgi_texture *tex;

typedef struct {
float d;
float posx;
float posy;
long lt; 
} background_states;

background_states bgs;

void guiProjection(int x, int y) {
  checkGLError("gui.c guiProj - start");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /*glOrtho(0, 0, x, y, -1, 1); */
  checkGLError("gui.c guiProj - proj");
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(game->screen->vp_x, game->screen->vp_y,
	     x, y);
  checkGLError("gui.c guiProj - end");
}

#define GUI_BLUE 0.3
void drawGui() {
  float x, y, w, h;
  float y1, y2;
  float a, b1, b2, c1, c2;
  float alpha;

#define N 20.0
  checkGLError("gui.c displayGui - before clear");
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  guiProjection(game->screen->vp_w, game->screen->vp_h);

  glBegin(GL_QUADS);
  c1 = 0.25; c2 = 0.75;
  glColor3f(c1, c1, GUI_BLUE * c1);
  glVertex2f(-1, -1);
  glColor3f(c2, c2, GUI_BLUE * c2);
  glVertex2f(1, -1);
  glVertex2f(1, 1);
  glColor3f(c1, c1, GUI_BLUE * c1);
  glVertex2f(-1, 1);
  glEnd();

  for(y1 = -1; y1 < 1; y1 += 2 / N) {
    y2 = y1 + 2 / N;
    for(x = -1; x < 1; x += 2 / N) {
      c1 = (x + 1) / 2;
      c2 = (x + 2 / N + 1) / 2;

      c1 = c1 / 2 + 0.25;
      c2 = c2 / 2 + 0.25;
      /* printf("using color %.2f\n", c); */
      
      glBegin(GL_QUADS);
      a = x + sin(bgs.d) / 10;
      /* b = x + cos(d) / 10 + 2 / N; */
      b1 = x + 2 / N;
      b2 = b1 + cos(bgs.d) / 10;
      /* printf("corners: (%.2f %.2f) (%.2f %.2f)\n", a, 0.0, b,
	 (float)yres[current]); */
      glColor3f(c1, c1, GUI_BLUE * c1);
      glVertex2f(a, y1);
      glColor3f(c2, c2, GUI_BLUE * c2);
      glVertex2f(b1, y1);
      glVertex2f(b2, y2);
      glColor3f(c1, c1, GUI_BLUE * c1);
      glVertex2f(a, y2);
      glEnd();
    }
  }

  x = bgs.posx;
  y = bgs.posy;
  w = 1;
  h = w/4;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, game->screen->texGui);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  checkGLError("gui.c - displayGui");

  alpha = (sin(bgs.d - M_PI / 2) + 1) / 2;
  glColor4f(1.0, 1.0, 0.0, alpha);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(x, y);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(x + w, y);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(x + w, y + h);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(x, y + h);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0, 0.0, 1.0);
  drawMenu(game->screen);

  if(game->settings->mouse_warp)
    mouseWarp();
}

void drawGuiBackground() {
  checkGLError("gui background start");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, game->screen->texGui);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

  glTexCoord2f(0.0, 0.0);
  glVertex2f(0, 0);

  glTexCoord2f(1.0, 0.0);
  glVertex2f(game->screen->vp_w, 0);

  glTexCoord2f(1.0, .75);
  glVertex2f(game->screen->vp_w, game->screen->vp_h);

  glTexCoord2f(0.0, .75);
  glVertex2f(0, game->screen->vp_h);

  glEnd();
  glDisable(GL_TEXTURE_2D);
  checkGLError("gui background end");
}
  
void displayGui() {
  /* drawGui(); */
  drawGuiBackground();
  drawMenu(game->screen);

  SystemSwapBuffers();  
}

void displayConfigure() {
  char message[] = "Press a key for this action!";
  drawGui();
  rasonly(game->screen);
  glColor3f(0.0, 0.0, 0.0);
  drawText(game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}

void idleGui() {
  float delta;
  long now;

#ifdef SOUND
  soundIdle();
#endif

  now = SystemGetElapsedTime();
  delta = now - bgs.lt;
  bgs.lt = now;
  delta /= 1000.0;
  bgs.d += delta;
  /* printf("%.5f\n", delta); */
  
  if(bgs.d > 2 * M_PI) { 
    bgs.d -= 2 * M_PI;
    bgs.posx = 1.0 * (float)rand() / (float)RAND_MAX - 1;
    bgs.posy = 1.5 * (float)rand() / (float)RAND_MAX - 1;
  }

  SystemPostRedisplay();
}

void keyboardConfigure(int key, int x, int y) {
  *configureKeyEntry = key;
  initMenuCaption(configureKeyMenu);
  /* todo: update menu caption */

  restoreCallbacks();
}

void keyboardGui(int key, int x, int y) {
  int i;
  switch(key) {
  case 27:
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_action);
#endif
    if(pCurrent->parent == NULL)
      restoreCallbacks();
    else
      pCurrent = pCurrent->parent;
    break;
  case 13: case ' ':
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_action);
#endif
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight));
    break;
  case 'q': exit(0); break;
  case 'l':
    printf("%d entries:\n", pCurrent->nEntries);
    for(i = 0; i < pCurrent->nEntries; i++)
      printf("printing '%s' - %d entries\n",
	     ((Menu*)*(pCurrent->pEntries + i))->szName,
	     ((Menu*)*(pCurrent->pEntries + i))->nEntries);
    break;
  case SYSTEM_KEY_DOWN:
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_highlight);
#endif
    pCurrent->iHighlight = (pCurrent->iHighlight + 1) % pCurrent->nEntries;
    break;
  case SYSTEM_KEY_UP:
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_highlight);
#endif
    pCurrent->iHighlight = (pCurrent->iHighlight - 1) % pCurrent->nEntries;
    if(pCurrent->iHighlight < 0)
      pCurrent->iHighlight = pCurrent->nEntries - 1;
    break;
  case SYSTEM_KEY_F12: doScreenShot(); break;
  default: printf("got key %d\n", key);
  }
}

void initGui() {
  /* init states */
  bgs.d = 0;
  bgs.posx = -1;
  bgs.posy = -1;
  bgs.lt = SystemGetElapsedTime();

  if(pCurrent == 0) {
    pCurrent = *pMenuList; /* erstes Menu ist RootMenu - Default pCurrent */
    pCurrent->iHighlight = 0;
  }

  /* rasonly(game->screen); */
}

void exitGui() {
}

void initGLGui() {
  glShadeModel(GL_SMOOTH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

}

callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, exitGui, initGLGui
};

callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, exitGui, initGLGui
};
