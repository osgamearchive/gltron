#include "game/gltron.h"

static int coffset;

char *credits[] = {
  "",
  "   GLtron is written by " "\x03" "4" "Andreas Umbach",
  "",
  " Contributors:",
  " Programming: Darrell Walisser  Nicolas Deniaud",
  "              Todd Kirby  Andy Howe  Jon Atkins",
  " Art:         Nicolas Zimmermann",
  "              Charles Babbage       Tracy Brown"
  "              Tyler Esselstrom       Allen Bond",
  " Music:       Peter Hajba",
  " Sound:       Damon Law",
  "",
  "Additional Thanks to:",
  "Xavier Bouchoux     Mike Field      Steve Baker",
  "Jean-Bruno Richard             Andrey Zahkhatov",
  "Bjonar Henden   Shaul Kedem    Jonas Gustavsson",
  "Mattias Engdegard     Ray Kelm     Thomas Flynn",
  "Martin Fierz    Joseph Valenzuela   Ryan Gordon",
  "Sam Lantinga                   Patrick McCarthy",
  "",
  "Thanks to my sponsors:",
  "  3dfx:              Voodoo5 5500 graphics card",
  "  Right Hemisphere:  3D exploration software",
  NULL
};

void mouseCredits (int buttons, int state, int x, int y)
{
	if ( state == SYSTEM_MOUSEPRESSED ) {
		SystemExit();
		exit(0);
	}
}

void keyCredits(int state, int k, int x, int y)
{
	if(state == SYSTEM_KEYSTATE_UP)
		return;
  SystemExit();
	exit(0);
}

void idleCredits(void) {
  scripting_RunGC();
  SystemPostRedisplay();
}

void drawCredits(void) {
  int time;
  int x, y;
  int h;
  int i;
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 } };
  time = SystemGetElapsedTime() - coffset;

  glClearColor(.0, .0, .0, .0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(gScreen);
  h = gScreen->vp_h / (24 * 3 / 2);
  for(i = 0; i < time / 250; i++) {
    glColor3fv(colors[i % 2]);
    if(credits[i] == NULL) 
      break;
    x = 10;
    y = gScreen->vp_h - 3 * h * (i + 1) / 2;
    drawText(gameFtx, x, y, h, credits[i]);
  }
}
void displayCredits(void) {
  drawCredits();
  SystemSwapBuffers();
}

void initCredits(void) {
  coffset = SystemGetElapsedTime();
}

Callbacks creditsCallbacks = { 
  displayCredits, idleCredits, keyCredits, initCredits, 
  NULL, NULL, mouseCredits, NULL, "credits"
};
