#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gltron.h"

/* FIXME: "ignored playMenuFX" */
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
  Sound_idle();
#endif

  scripting_RunGC();
	SDL_Delay(10);
  SystemPostRedisplay(); /* animate menu */
}

void keyboardConfigure(int key, int x, int y) {
  scripting_RunFormat("settings.keys[ configure_player ][ configure_event ]"
		      "= %d", key);
	SystemExitLoop(RETURN_PROMPT_ESCAPE);
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
			SystemExitLoop(RETURN_GUI_ESCAPE);
    }
    break;
  case ' ': 
  case SYSTEM_KEY_RETURN:
	case SYSTEM_JOY_BUTTON_0:
	case SYSTEM_JOY_BUTTON_1:
	case SYSTEM_JOY_BUTTON_2:
	case SYSTEM_JOY_BUTTON_3:
	case SYSTEM_JOY_BUTTON_4:
	case SYSTEM_JOY_BUTTON_5:
	case SYSTEM_JOY_BUTTON_6:
	case SYSTEM_JOY_BUTTON_7:
	case SYSTEM_JOY_BUTTON_8:
	case SYSTEM_JOY_BUTTON_9:
	case SYSTEM_JOY_BUTTON_0 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_1 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_2 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_3 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_4 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_5 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_6 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_7 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_8 + SYSTEM_JOY_OFFSET:
	case SYSTEM_JOY_BUTTON_9 + SYSTEM_JOY_OFFSET:
    scripting_Run("Menu.Action()");
    break;
  case SYSTEM_KEY_UP:
	case SYSTEM_JOY_UP:
	case SYSTEM_JOY_UP + SYSTEM_JOY_OFFSET:
    scripting_Run("Menu.Previous()");
    break;
  case SYSTEM_KEY_DOWN:
	case SYSTEM_JOY_DOWN:
	case SYSTEM_JOY_DOWN + SYSTEM_JOY_OFFSET:
    scripting_Run("Menu.Next()");
    break;
  case SYSTEM_KEY_RIGHT:
	case SYSTEM_JOY_RIGHT:
		case SYSTEM_JOY_RIGHT + SYSTEM_JOY_OFFSET:
    scripting_Run("Menu.Right()");
    break;
  case SYSTEM_KEY_LEFT:
	case SYSTEM_JOY_LEFT:
		case SYSTEM_JOY_LEFT + SYSTEM_JOY_OFFSET:
    scripting_Run("Menu.Left()");
    break;
  case SYSTEM_KEY_F11: doBmpScreenShot(game->screen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(game->screen); break;
  default: 
    // printf("got key %d\n", key);
		break;
  }
  free(pMenuName);
  SystemPostRedisplay();
}

void initGui() {
  SystemUnhidePointer();

  updateSettingsCache();

  menutime = SystemGetElapsedTime();
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
	
void guiMouse(int buttons, int state, int x, int y) {
  fprintf(stderr, "Mouse buttons: %d, State %d, Position (%d, %d)\n",
	  buttons, state, x, y); 

  /* fprintf(stderr, "testing for state == %d\n", SYSTEM_MOUSEPRESSED); */
  if (state == SYSTEM_MOUSEPRESSED) {	
#ifdef SOUND
    if(getSettingi("playEffects"))
      playMenuFX(fx_action);
#endif
    SystemPostRedisplay();
  }
}

void guiMouseMotion(int mx, int my) {
  /* TODO: add mouse cursor, highlighted areas, etc. */
}

callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, exitGui, initGLGui,
  NULL, NULL, "configure"
};

callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, exitGui, initGLGui, 
  guiMouse, guiMouseMotion, "gui"
};

