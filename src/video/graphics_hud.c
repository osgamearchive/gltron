#include "video/video.h"
#include "game/game.h"

#include "base/nebu_math.h"
#include "scripting/nebu_scripting.h"

#include <lua.h>
#include <lualib.h>

void drawHudComponent(const char *s, nebu_2d *pMask) {
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.9);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 255);
		glStencilOp(GL_KEEP, GL_INCR, GL_INCR);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		nebu_2d_Draw(pMask);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);

		// draw gauge where stencil is set
		glStencilFunc(GL_LESS, 0, 255);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		scripting_Run(s);
		
		glDisable(GL_STENCIL_TEST);
}

void drawHUD(Player *p, PlayerVisual *pV) {
	if (gSettingsCache.show_scores) {
		char tmp[10]; /* hey, they won't reach such a score */
		sprintf(tmp, "%d", p->data->score);
		
		rasonly(&pV->display);
		glColor4f(1.0, 1.0, 0.2f, 1.0);
		drawText(gameFtx, 5, 5, 32, tmp);
	}

	if(gSettingsCache.show_ai_status &&
		 p->ai->active == AI_COMPUTER) {
		char ai[] = "computer player";

		rasonly(&pV->display);
		glColor3f(1.0, 1.0, 1.0);
		drawText(gameFtx, 
						 pV->display.vp_w / 4, 10, 
						 pV->display.vp_w / (2 * strlen(ai)), ai);
	}
}


void drawPause(Visual *display) {
  char pause[] = "Game is paused";
  char winner[] = "Player %d wins!";
  char nowinner[] = "No one wins!";
  char buf[100];
  char *message;
  static float d = 0;
  static float lt = 0;
  float delta;
  int now;

  now = nebu_Time_GetElapsed();
  delta = now - lt;
  lt = now;
  delta /= 500.0;
  d += delta;
  /* printf("%.5f\n", delta); */
  
  if (d > 2 * PI) { 
    d -= 2 * PI;
  }

  if ((game->pauseflag & PAUSE_GAME_FINISHED) && game->winner != -1) {
    if (game->winner >= -1) {

      float* player_color = gPlayerVisuals[game->winner].pColorAlpha;

      /* 
         make the 'Player wins' message oscillate between 
         white and the winning bike's color 
       */
      glColor3f((player_color[0] + ((sinf(d) + 1) / 2) * (1 - player_color[0])),
                (player_color[1] + ((sinf(d) + 1) / 2) * (1 - player_color[1])),
                (player_color[2] + ((sinf(d) + 1) / 2) * (1 - player_color[2]))); 

      message = buf;
      sprintf(message, winner, game->winner + 1);
    } else {
      glColor3d(1.0, (sin(d) + 1) / 2, (sin(d) + 1) / 2);
      message = nowinner;
    }
  } else {
    glColor3d(1.0, (sin(d) + 1) / 2, (sin(d) + 1) / 2);
    message = pause;
  }

  rasonly(gScreen);
  drawText(gameFtx, display->vp_w / 6, 20, 
	   display->vp_w / (6.0f / 4.0f * strlen(message)), message);
}

void drawScore(Player *p, Visual *d) {
}

  
void drawFPS(Visual *d) {
#define FPS_HSIZE 20
  /* draws FPS in upper left corner of Display d */
  static int fps_h[FPS_HSIZE];
  static int pos = -FPS_HSIZE;
  static int fps_min = 0;
  static int fps_avg = 0;

  char tmp[20];
  int diff;

  rasonly(d);
  diff = (game2->time.dt > 0) ? game2->time.dt : 1;

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
    }
  }

  sprintf(tmp, "average FPS: %d", fps_avg);
  glColor4f(1.0, 0.4f, 0.2f, 1.0);
  drawText(gameFtx, d->vp_w - 180, d->vp_h - 20, 10, tmp);
  sprintf(tmp, "minimum FPS: %d", fps_min);
  drawText(gameFtx, d->vp_w - 180, d->vp_h - 35, 10, tmp);
  sprintf(tmp, "triangles: %d", polycount);
  drawText(gameFtx, d->vp_w - 180, d->vp_h - 50, 10, tmp);
}


void drawConsoleLines(char *line, int call) {
#define CONSOLE_SIZE 15
#define CONSOLE_X_OFF 20
  int size = CONSOLE_SIZE;
  int length;
  /* fprintf(stdout, "%s\n", line); */
  length = strlen(line);
  while(length * size > gScreen->vp_w / 2 - CONSOLE_X_OFF)
    size--;
    
  if(*line != 0) 
    drawText(gameFtx, CONSOLE_X_OFF, gScreen->vp_h - 20 * (call + 1),
	     size, line);
}

void drawConsole(Visual *d) {
  int lines;
  rasonly(d);
  glColor3f(1.0, 0.3f, 0.3f);
  
  if (gSettingsCache.softwareRendering) { 
    lines = 1;
  } else if (gScreen->vp_h < 600) {
    lines = 3;
  } else {
    lines = 5;
  }
  
  consoleDisplay(drawConsoleLines, lines);
}

/* new hud stuff starts here */

void drawCircle(float phiStart, float phiEnd, 
								int nSegments, 
								float r1, float r2,
								float *c1, float *c2, float *c3, float *c4);
void drawRect(float width, float height, float *colors);

static void getColor(lua_State *l, float *out) {
	char *names[] = { "r", "g", "b" };
	int i;
	for(i = 0; i < 3; i++) {
		lua_pushstring(l, names[i]);
		lua_gettable(l, -2);
		out[i] = lua_tonumber(l, -1);
		lua_pop(l, 1); // result
	}
	lua_pop(l, 1); // table
}

int c_drawRectangle(lua_State *l) {
	float width = 0, height = 0;
	float colors[12];

	getColor(l, colors + 9);
	getColor(l, colors + 6);
	getColor(l, colors + 3);
	getColor(l, colors + 0);
	
	height = lua_tonumber(l, -1);		lua_pop(l, 1);
	width = lua_tonumber(l, -1);		lua_pop(l, 1);
	
	drawRect(width, height, colors);

	return 0;
}
	
int c_drawCircle(lua_State *l) {
	float phiStart = 0, phiEnd = 0;
	int nSegments = 0;
	float r1 = 0, r2 = 0;

	float c1[3] = { 0, 0, 0 };
	float c2[3] = { 0, 0, 0 };
	float c3[3] = { 0, 0, 0 };
	float	c4[3] = { 0, 0, 0 };
	
	getColor(l, c4);
	getColor(l, c3);
	getColor(l, c2);
	getColor(l, c1);

	r2 = lua_tonumber(l, -1);		lua_pop(l, 1);
	r1 = lua_tonumber(l, -1);		lua_pop(l, 1);
	nSegments = (int) lua_tonumber(l, -1);		lua_pop(l, 1);
	phiEnd = lua_tonumber(l, -1);		lua_pop(l, 1);
	phiStart = lua_tonumber(l, -1);		lua_pop(l, 1);

	drawCircle(phiStart, phiEnd, nSegments, r1, r2, c1, c2, c3, c4);

	return 0;
}

int c_translate(lua_State *l) {
	float x = 0, y = 0, z = 0;
	z = lua_tonumber(l, -1);		lua_pop(l, 1);
	y = lua_tonumber(l, -1);		lua_pop(l, 1);
	x = lua_tonumber(l, -1);		lua_pop(l, 1);
	glTranslatef(x,y,z);

	return 0;
}

int c_pushMatrix(lua_State *l) {
	glPushMatrix();
	return 0;
}

int c_popMatrix(lua_State *l) {
	glPopMatrix();
	return 0;
}

void rgb_interpolate(float *color, float t, float *c1, float *c2) {
	int i;
	// printf("%.2f\n", t);
	for(i = 0; i < 3; i++) {
		color[i] = c1[i] * (1 - t) + c2[i] * t;
	}
}

void drawCircle(float phiStart, float phiEnd, 
								int nSegments, 
								float r1, float r2,
								float *c1, float *c2, float *c3, float *c4) {
	float *pVertices;
	float *pColors;
	unsigned int* pIndices;

	int i;

	if(nSegments < 1)
		return;

	pVertices = (float*) malloc( (nSegments + 1) * 2 * 3 * sizeof(float) );
	pColors = (float*) malloc( (nSegments + 1) * 2 * 3 * sizeof(float) );

	for(i = 0; i < nSegments + 1; i++) {
		float t = i / (float)nSegments;
		float rad = (1 - t) * phiStart + t * phiEnd;
		pVertices[3 * (2 * i + 0) + 0] = cos(rad) * r1;
		pVertices[3 * (2 * i + 0) + 1] = sin(rad) * r1;
		pVertices[3 * (2 * i + 0) + 2] = 0;

		pVertices[3 * (2 * i + 1) + 0] = cos(rad) * r2;
		pVertices[3 * (2 * i + 1) + 1] = sin(rad) * r2;
		pVertices[3 * (2 * i + 1) + 2] = 0;
		
		rgb_interpolate(pColors + 3 * (2 * i + 0), t, c1, c2);
		rgb_interpolate(pColors + 3 * (2 * i + 1), t, c3, c4);
	}
	
	pIndices = (unsigned int*) 
		malloc( (nSegments) * 2 * 3 * sizeof(unsigned int) );

	for(i = 0; i < nSegments; i++) {
		pIndices[3 * (2 * i + 0) + 0] = (i + 0) * 2 + 0;
		pIndices[3 * (2 * i + 0) + 1] = (i + 0) * 2 + 1;
		pIndices[3 * (2 * i + 0) + 2] = (i + 1) * 2 + 0;

		pIndices[3 * (2 * i + 1) + 0] = (i + 1) * 2 + 0;
		pIndices[3 * (2 * i + 1) + 1] = (i + 0) * 2 + 1;
		pIndices[3 * (2 * i + 1) + 2] = (i + 1) * 2 + 1;
	}
	
	glVertexPointer(3, GL_FLOAT, 0, pVertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, pColors);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glDrawElements(GL_TRIANGLES, 3 * 2 * nSegments, GL_UNSIGNED_INT, pIndices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	free(pVertices);
	free(pColors);
	free(pIndices);
	
}

void drawRect(float width, float height, 
							float *colors) {
	float vertices[12] = { 
		0, 0, 0, 
		0, height, 0,
		width, height, 0,
		width, 0, 0
	};
		
	unsigned int indices[] = { 
		0, 1, 2, 0, 2, 3
	};

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

