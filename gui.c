#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gltron.h"

#warning "ignored playMenuFX"

void playMenuFX(int foo) { }

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

#if 0
  if(game->settings->softwareRendering) {
    glRasterPos2i(0, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(game->screen->vp_w, game->screen->vp_h,
		 GL_RGB, GL_UNSIGNED_BYTE,
		 game->screen->pixelGui);
  } else {
#endif
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_GUI]);
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
#if 0
  }
#endif
}

void drawGuiLogo() {
  float pos[] = { 512 - 10 - 256, 384 - 64 };
  float size[] = { 256, 64 };
  float glpos = 64;
  float glsize = 32;
  float font_shift[] = { 0.5, 0.00 };

  checkGLError("gui logo start");
  
  rasonly(game->screen);

  pos[0] *= game->screen->vp_w / 512.0;
  pos[1] *= game->screen->vp_h / 384.0;
  size[0] *= game->screen->vp_w / 512.0;
  size[1] *= game->screen->vp_h / 384.0;
  glpos *= game->screen->vp_w / 512.0;
  glsize *= game->screen->vp_w / 512.0;
  
  glEnable(GL_TEXTURE_2D);

  if(getSettingi("show_gl_logo") == 1) {
    glPushMatrix();
    glTranslatef(pos[0] - glpos + glsize * font_shift[0], 
		 pos[1] + glsize * font_shift[1], 0);
    glScalef(glsize, glsize, glsize);
    glColor3f(0.2, 0.4, 0.8);
    ftxRenderString(gameFtx, "gl", 2);
    glPopMatrix();
  }

  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_LOGO]);
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
#ifdef SOUND
  soundIdle();
#endif

  scripting_RunGC();
  SystemPostRedisplay(); /* animate menu */
}

void keyboardConfigure(int key, int x, int y) {
  scripting_RunFormat("keys[ configure_player ][ configure_event ] = %d", key);
  restoreCallbacks();
}

void keyboardGui(int key, int x, int y) {
  char *pMenuName;
  scripting_Run("return Menu.current");
  scripting_GetStringResult(&pMenuName);

  switch(key) {
  case 27:
    if(strcmp(pMenuName, "RootMenu")) {
      scripting_Run("Menu.GotoParent()");
    } else {
      restoreCallbacks(); // back to game
      /* SystemQuit(); */ /* exit */
    }
    break;
  case ' ': 
  case SYSTEM_KEY_RETURN:
    scripting_Run("Menu.Action()");
    break;
  case SYSTEM_KEY_UP:
    scripting_Run("Menu.Previous()");
    break;
  case SYSTEM_KEY_DOWN:
    scripting_Run("Menu.Next()");
    break;
  case SYSTEM_KEY_RIGHT:
    scripting_Run("Menu.Right()");
    break;
  case SYSTEM_KEY_LEFT:
    scripting_Run("Menu.Left()");
  default: 
    printf("got key %d\n", key);
  }
  free(pMenuName);
  SystemPostRedisplay();
}

#if 0

void keyboardGui(int key, int x, int y) {
  int i;
  switch(key) {
  case 27:
#ifdef SOUND
    if(getSettingi("playEffects")) 
      playMenuFX(fx_action);
#endif
    if(pCurrent->parent == NULL)
      restoreCallbacks();
    else
      pCurrent = pCurrent->parent;
    break;
  case 13: case ' ':
#ifdef SOUND
    if(getSettingi("playEffects")) 
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
    /* case 'q': SystemExit(); break; */
  case SYSTEM_KEY_DOWN:
#ifdef SOUND
    if(getSettingi("playEffects"))
      playMenuFX(fx_highlight);
#endif
    pCurrent->iHighlight = (pCurrent->iHighlight + 1) % pCurrent->nEntries;
    break;
  case SYSTEM_KEY_UP:
#ifdef SOUND
    if(getSettingi("playEffects"))
      playMenuFX(fx_highlight);
#endif
    pCurrent->iHighlight = (pCurrent->iHighlight - 1) % pCurrent->nEntries;
    if(pCurrent->iHighlight < 0)
      pCurrent->iHighlight = pCurrent->nEntries - 1;
    break;
  case SYSTEM_KEY_F11: doBmpScreenShot(game->screen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(game->screen); break;
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
  SystemPostRedisplay();
}
#endif /* 0 */

void initGui() {
  SystemUnhidePointer();
  menutime = SystemGetElapsedTime();
#if 0
  if(pCurrent == NULL) {
    pCurrent = *pMenuList; /* erstes Menu ist RootMenu - Default pCurrent */
    pCurrent->iHighlight = 0;
  }
#endif
}

void exitGui() {
  glShadeModel( game->screen->shademodel );
}

void initGLGui() {
  glShadeModel(GL_FLAT);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  SystemPostRedisplay();
}

static int current_highlight = -1;
	
void guiMouse(int buttons, int state, int x, int y) {
  fprintf(stderr, "Mouse buttons: %d, State %d, Position (%d, %d)\n",
	  buttons, state, x, y); 

  /* fprintf(stderr, "testing for state == %d\n", SYSTEM_MOUSEPRESSED); */
  if (state == SYSTEM_MOUSEPRESSED) {	
#ifdef SOUND
    if(getSettingi("playEffects"))
      playMenuFX(fx_action);
#endif
#if 0	
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_ACTION);
#endif
    SystemPostRedisplay();
  }
}

void guiMouseMotion(int mx, int my) {
  /* fprintf(stderr, "Mouse motion at (%d, %d)\n", x, y); */
 
  /* I am using the calculation in drawMenu, perhaps
     these values should be bound to Menu structure in the future. */

  /* TODO: this is cut-and-paste from menu.c: VERY UGLY */
  int i;
  int x, y, size, lineheight;
  int hsize, vsize;
  int maxw = 0;

#define MENU_TEXT_START_X 0.08
#define MENU_TEXT_START_Y 0.40

#define MENU_WIDTH 0.80
#define MENU_HEIGHT 0.40

#define MENU_TEXT_LINEHEIGHT 1.5
	
  gDisplay *d = game->screen; 
	
  x = (int) (d->vp_w * MENU_TEXT_START_X);
  y = (int) (d->vp_h * MENU_TEXT_START_Y);

  /* transform mouse y-coordinate */

  my = d->vp_h - my;

#if 0
  /* new stuff: calculate menu dimensions */
  for(i = 0; i < pCurrent->nEntries; i++) {
    int len;
    len =  strlen(((Menu*)*(pCurrent->pEntries + i))->display.szCaption);
    if(len > maxw)
      maxw = len;
  }
  /* adjust size so menu fits into MENU_WIDTH/HEIGHT */

  hsize = (int) ((float)d->vp_w * MENU_WIDTH / (float)maxw );
  vsize = (int) ((float)d->vp_h * MENU_HEIGHT / 
		 ( (float)pCurrent->nEntries * MENU_TEXT_LINEHEIGHT));

  size = (hsize < vsize) ? hsize : vsize;

  lineheight = (int)( (float) size * MENU_TEXT_LINEHEIGHT);  

  for (i = 0; i < pCurrent->nEntries; i++) {
    if (current_highlight != i)
      if (my < y + lineheight && my > y ) {
	current_highlight = i;

#ifdef SOUND
	if(getSettingi("playEffects"))
	  playMenuFX(fx_highlight);
#endif
	pCurrent->iHighlight = i;
	SystemPostRedisplay ();
	break;	
      }
 		
    y -= lineheight;
  }
#endif
}

callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, exitGui, initGLGui,
  NULL, NULL
};

callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, exitGui, initGLGui, 
  guiMouse, guiMouseMotion
};




