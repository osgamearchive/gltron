#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "game/gltron.h"

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
  glViewport(gScreen->vp_x, gScreen->vp_y,
	     x, y);
  checkGLError("gui.c guiProj - end");
}

void drawGuiBackground(void) {
  checkGLError("gui background start");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(gScreen);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_GUI]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

  glTexCoord2f(0.0, 0.0);
  glVertex2f(0, 0);

  glTexCoord2f(1.0, 0.0);
  glVertex2i(gScreen->vp_w, 0);

  glTexCoord2f(1.0, .75);
  glVertex2i(gScreen->vp_w, gScreen->vp_h);

  glTexCoord2f(0.0, .75);
  glVertex2i(0, gScreen->vp_h);

  glEnd();
}

void drawGuiLogo(void) {
  float pos[] = { 512 - 10 - 320, 384 - 80 };
  float size[] = { 320, 80 };
  float glpos = 64;
  float glsize = 32;

  checkGLError("gui logo start");
  
  rasonly(gScreen);

  pos[0] *= gScreen->vp_w / 512.0f;
  pos[1] *= gScreen->vp_h / 384.0f;
  size[0] *= gScreen->vp_w / 512.0f;
  size[1] *= gScreen->vp_h / 384.0f;
  glpos *= gScreen->vp_w / 512.0f;
  glsize *= gScreen->vp_w / 512.0f;
  
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_LOGO]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

	{ 
		// float texy = 1.0f - 256.0f / 320.0f;
		float texy = 0.0f;
		glTexCoord2f(0.0, texy);
		glVertex2f(pos[0], pos[1]);

		glTexCoord2f(1.0, texy);
		glVertex2f(pos[0] + size[0], pos[1]);

		glTexCoord2f(1.0, 1.0);
		glVertex2f(pos[0] + size[0], pos[1] + size[1]);

		glTexCoord2f(0.0, 1.0);
		glVertex2f(pos[0], pos[1] + size[1]);
	}
	
  glEnd();

  glDisable(GL_BLEND);

  checkGLError("gui background end");
}
  
void displayGui(void) {
  drawGuiBackground();
  drawGuiLogo();
  drawMenu(gScreen);

  SystemSwapBuffers();  
}

void displayConfigure(void) {
  char message[] = "Press a key for this action!";
  drawGuiBackground();
  drawGuiLogo();
  drawMenu(gScreen);

  rasonly(gScreen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, gScreen->vp_w / 6, 20,
	   gScreen->vp_w / (6.0f / 4.0f * strlen(message)), message);
  SystemSwapBuffers();
}

void idleGui(void) {
	Sound_idle();
	scripting_RunGC();
	SDL_Delay(10);
	Video_Idle();
	Input_Idle();
	SystemPostRedisplay(); /* animate menu */
}

void keyboardConfigure(int state, int key, int x, int y) {
	if(state == SYSTEM_KEYSTATE_DOWN) {
		scripting_RunFormat("settings.keys[ configure_player ][ configure_event ]"
												"= %d", key);
		SystemExitLoop(RETURN_PROMPT_ESCAPE);
	}
}

void keyboardGui(int state, int key, int x, int y) {
  char *pMenuName;

	if(state == SYSTEM_KEYSTATE_UP)
		return;

  scripting_Run("return Menu.current");
  scripting_GetStringResult(&pMenuName);

  switch(key) {
  case 27:
    if(strcmp(pMenuName, "RootMenu")) {
			// not in the root menu -> go up one menu
      scripting_Run("Menu.GotoParent()");
    } else {
			// is a game already in process? then resume
			if(game->pauseflag != PAUSE_NO_GAME)
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
  case SYSTEM_KEY_F11: doBmpScreenShot(gScreen); break;
  case SYSTEM_KEY_F12: doPngScreenShot(gScreen); break;
  default: 
    // printf("got key %d\n", key);
		break;
  }
  free(pMenuName);
}

void initGui(void) {
  SystemUnhidePointer();

  updateSettingsCache();
}

void exitGui(void) {
  glShadeModel( GL_SMOOTH );
}

void initGLGui(void) {
  glShadeModel(GL_FLAT);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
}
	
void guiMouse(int buttons, int state, int x, int y) {
  fprintf(stderr, "Mouse buttons: %d, State %d, Position (%d, %d)\n",
	  buttons, state, x, y); 

  /* fprintf(stderr, "testing for state == %d\n", SYSTEM_MOUSEPRESSED); */
  if (state == SYSTEM_MOUSEPRESSED) {	
    if(getSettingi("playEffects"))
      playMenuFX(fx_action);
  }
}

void guiMouseMotion(int mx, int my) {
  /* TODO: add mouse cursor, highlighted areas, etc. */
}

Callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, exitGui, initGLGui,
  NULL, NULL, "configure"
};

Callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, exitGui, initGLGui, 
  guiMouse, guiMouseMotion, "gui"
};

