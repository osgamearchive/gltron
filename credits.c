#include "gltron.h"

static int coffset;

char *credits[] = {
  "",
  "   GLtron is written by " "\x03" "4" "Andreas Umbach",
  "",
  " Contributors:",
  " Programming: Darrell Walisser       Jon Atkins",
  "              Todd Kirby",
  " Art:         Nicolas Zimmermann",
  "              Charles Babbage       Tracy Brown"
  "              Tyler Esselstrom       Allen Bond",
  " Music:       Peter Hajba",
  " Sound:       Damon Law",
  "",
  "Additional Thanks to:",
  "Xavier Bouchoux     Mike Field      Steve Baker",
  "Jean-Bruno Richard             Andrey Zahkhatov",
  "Bjonar Henden   Shaul Kedem    Darrell Walisser",
  "Jonas Gustavsson  Thomas Flynn  Charles Babbage",
  "Nicolas Zimmermann  Mattias Engdegard  Ray Kelm",
  "Martin Fierz    Joseph Valenzuela   Ryan Gordon",
  "Sam Lantinga                   Patrick McCarthy",
  "",
  "and everyone else, who gave feedback ",
  "and valuable advice",
  NULL
};

void mouseCredits (int buttons, int state, int x, int y)
{
    if ( state == SYSTEM_MOUSEPRESSED )
        SystemExit();
}

void keyCredits(int k, int x, int y)
{
  SystemExit();
}

void idleCredits() {
  scripting_RunGC();
  SystemPostRedisplay();
}

void drawCredits() {
  int time;
  int x, y;
  int h;
  int i;
  float colors[][3] = { { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 } };
  time = SystemGetElapsedTime() - coffset;

  glClearColor(.0, .0, .0, .0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);
  h = game->screen->vp_h / (24 * 1.5);
  for(i = 0; i < time; i++) {
    glColor3fv(colors[i % 2]);
    if(credits[i] == NULL) 
      break;
    x = 10;
    y = game->screen->vp_h - 1.5 * h * (i + 1);
    drawText(gameFtx, x, y, h, credits[i]);
  }
}
void displayCredits() {
  drawCredits();
  SystemSwapBuffers();
}

void initCredits() {
  coffset = SystemGetElapsedTime();
}


callbacks creditsCallbacks = { 
  displayCredits, idleCredits, keyCredits, initCredits, 
  NULL, NULL, mouseCredits, NULL
};
