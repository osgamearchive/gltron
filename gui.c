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
}

void drawGuiLogo() {
  float pos[] = { 512 - 10 - 256, 384 - 64 };
  float size[] = { 256, 64 };
  float glpos = 64;
  float glsize = 32;
  float font_shift[] = { 0.5, 0.12 };

  checkGLError("gui logo start");
  
  rasonly(game->screen);

  pos[0] *= game->screen->vp_w / 512.0;
  pos[1] *= game->screen->vp_h / 384.0;
  size[0] *= game->screen->vp_w / 512.0;
  size[1] *= game->screen->vp_h / 384.0;
  glpos *= game->screen->vp_w / 512.0;
  glsize *= game->screen->vp_w / 512.0;
  
  glEnable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(pos[0] - glpos + glsize * font_shift[0], 
	       pos[1] + glsize * font_shift[1], 0);
  glScalef(glsize, glsize, glsize);
  glColor3f(0.2, 0.4, 0.8);
  ftxRenderString(guiFtx, "gl", 2);
  glPopMatrix();
  
  glBindTexture(GL_TEXTURE_2D, game->screen->texLogo);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

  glTexCoord2f(0.0, 0.0);
  glVertex2f(pos[0], pos[1]);

  glTexCoord2f(1.0, 0.0);
  glVertex2f(pos[0] + size[0], pos[1]);

  glTexCoord2f(1.0, 1.0);
  glVertex2f(pos[0] + size[0], pos[1] + size[1]);

  glTexCoord2f(0.0, 1.0);
  glVertex2f(pos[0], pos[1] + size[1]);

  glEnd();

  glDisable(GL_BLEND);

  checkGLError("gui background end");
}
  
void displayGui() {
  drawGuiBackground();
  drawGuiLogo();
  drawMenu(game->screen);

  SystemSwapBuffers();  
}

void displayConfigure() {
  char message[] = "Press a key for this action!";
  drawGuiBackground();
  drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
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
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_ACTION);
    break;
  case SYSTEM_KEY_LEFT:
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_LEFT);
    break;
  case SYSTEM_KEY_RIGHT:
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_RIGHT);
    break;
  case 'q': exit(0); break;
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
    /* debug code follows */
  case 'l':
    printf("%d entries:\n", pCurrent->nEntries);
    for(i = 0; i < pCurrent->nEntries; i++)
      printf("printing '%s' - %d entries\n",
	     ((Menu*)*(pCurrent->pEntries + i))->szName,
	     ((Menu*)*(pCurrent->pEntries + i))->nEntries);
    break;
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
  glShadeModel( game->screen->shademodel );
}

void initGLGui() {
  glShadeModel(GL_SMOOTH);

  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

}

callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, exitGui, initGLGui
};

callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, exitGui, initGLGui
};
