#include "video/video.h"
#include "game/game.h"

void drawWalls(void) {
#undef WALL_H
#define WALL_H 48
  float t;
  float h;

  t = game2->rules.grid_size / 240.0f;
  if (gSettingsCache.stretch_textures) {
    h = t * WALL_H;
    t = 1.0;
  } else h = WALL_H;

  glColor4f(1.0, 1.0, 1.0, 1.0);

  /*
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  */
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
#define T_TOP 1.0f
  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_WALL1]);
  glBegin(GL_QUADS);
  glTexCoord2f(t, 0.0); glVertex3f(0.0, 0.0, 0.0);
  glTexCoord2f(t, T_TOP); glVertex3f(0.0, 0.0, h);
  glTexCoord2f(0.0, T_TOP); glVertex3f(game2->rules.grid_size, 0.0, h);
  glTexCoord2f(0.0, 0.0); glVertex3f(game2->rules.grid_size, 0.0, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_WALL2]);
  glBegin(GL_QUADS);
  glTexCoord2f(t, 0.0); glVertex3f(game2->rules.grid_size, 0.0, 0.0);
  glTexCoord2f(t, T_TOP); glVertex3f(game2->rules.grid_size, 0.0, h);
  glTexCoord2f(0.0, T_TOP); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, h);
  glTexCoord2f(0.0, 0.0); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_WALL3]);
  glBegin (GL_QUADS);
  glTexCoord2f(t, 0.0); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, 0.0);
  glTexCoord2f(t, T_TOP); 
  glVertex3f(game2->rules.grid_size, game2->rules.grid_size, h);
  glTexCoord2f(0.0, T_TOP); glVertex3f(0.0, game2->rules.grid_size, h);
  glTexCoord2f(0.0, 0.0); glVertex3f(0.0, game2->rules.grid_size, 0.0);
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_WALL4]);
  glBegin(GL_QUADS);
  glTexCoord2f(t, 0.0); glVertex3f(0.0, game2->rules.grid_size, 0.0);
  glTexCoord2f(t, T_TOP); glVertex3f(0.0, game2->rules.grid_size, h);
  glTexCoord2f(0.0, T_TOP); glVertex3f(0.0, 0.0, h);
  glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0); 
#undef T_TOP
  glEnd();
  polycount += 8;

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
}

/*!
 * drawFloorTextured 
 *
 * \return number of polygons drawn
 *
 * Draws the arena floor covered with a repeating floor texture
 */

int drawFloorTextured(int grid_size, GLuint texture) {
  int i, j, l, t;
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

    /* there are some strange clipping artefacts on some renderers */
    /* try subdividing things... */

    glColor4f(1.0, 1.0, 1.0, 1.0);
    l = grid_size / 4;
    t = l / 12;
    
    //  glBegin(GL_QUADS);
    for (i = 0; i < grid_size; i += l) {
      glBegin(GL_QUADS);
      for (j = 0; j < grid_size; j += l) {
        glTexCoord2i(0, 0);
        glVertex2i(i, j);
        glTexCoord2i(t, 0);
        glVertex2i(i + l, j);
        glTexCoord2i(t, t);
        glVertex2i(i + l, j + l);
        glTexCoord2i(0, t);
        glVertex2i(i, j + l);
      }
      glEnd();
    }
    // glEnd();
   
    glDisable(GL_TEXTURE_2D);
    return grid_size * grid_size;
}

void drawFloorGrid(int grid_size,  int line_spacing, 
                   float line_color[4], float square_color[4]) {
  int i, j;

  glColor3fv(line_color);
  
  glFogfv(GL_FOG_COLOR, square_color);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogi(GL_FOG_START, 100);
  glFogi(GL_FOG_END, 350);

  glEnable(GL_FOG);

  glBegin(GL_LINES);
  for (i = 0; i < grid_size; i += line_spacing) {
    for (j = 0; j < grid_size; j += line_spacing) {
      glVertex3i(i, j, 0);
      glVertex3i(i + line_spacing, j, 0);
      glVertex3i(i, j, 0);
      glVertex3i(i, j + line_spacing, 0);
    }
  }
  glEnd();

  glDisable(GL_FOG);
}
