#include "gltron.h"

void enableSkyboxTexture() {
  glEnable(GL_TEXTURE_2D);
}

void disableSkyboxTexture() {
  glDisable(GL_TEXTURE_2D);
}

void bindSkyboxTexture(int index) {
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[ TEX_SKYBOX + index ]);
}

void skybox() {
  /* 
     matrices are: 
     projection: perspective projection 
     modelview: identity
  */
  
  /* these are the values for y == up, x == front */
  /* 
  float sides[6][4][3] = {
    { { 1, -1, -1 }, { 1, -1, 1 }, { 1, 1, 1 },  { 1, 1, -1 } }, // front
    { { 1, 1, -1 }, { 1, 1, 1 }, { -1, 1, 1 }, { -1, 1, -1 } }, // top
    { { -1, -1, -1 }, { 1, -1, -1 },  { 1, 1, -1 }, { -1, 1, -1 } }, // left
    { { 1, -1, 1 }, { -1, -1, 1 }, { -1, 1, 1 }, { 1, 1, 1 } }, // right
    { { -1, -1, -1 }, { 1, -1, -1 }, { 1, -1, 1 }, { -1, -1, 1 } }, // botton
    { { -1, -1, 1 }, { -1, -1, -1 }, { -1, 1, -1 }, { -1, 1, a1 } } // back
  };
  */

  /* these values are for z == up, x == front */
  float sides[6][4][3] = {
    { { 1, 1, -1 }, { 1, -1, -1 }, { 1, -1, 1 },  { 1, 1, 1 } }, /* front */
    { { 1, 1, 1 }, { -1, 1, 1 }, { -1, -1, 1 }, { 1, -1, 1 } }, /* top */
    { { -1, 1, -1 }, { 1, 1, -1 },  { 1, 1, 1 }, { -1, 1, 1 } }, /* left */
    { { 1, -1, -1 }, { -1, -1, -1 }, { -1, -1, 1 }, { 1, -1, 1 } }, /* right */
    { { -1, 1, -1 }, { -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 } }, /* botton */
    { { -1, -1, -1 }, { -1, 1, -1 }, { -1, 1, 1 }, { -1, -1, 1 } } /* back */
  };
    
  float uv[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
  int i, j;
  float d = game->settings->grid_size * 3;

  glEnable(GL_TEXTURE_2D);
  glDepthMask(GL_FALSE);
  glColor3f(1.0, 1.0, 1.0);
  for(i = 0; i < 6; i++) {
    bindSkyboxTexture(i);
    glBegin(GL_QUADS);
    for(j = 0; j < 4; j++) {
      glTexCoord2fv( uv[j] );
      glVertex3f( sides[i][j][0] * d, sides[i][j][1] * d, sides[i][j][2] * d );
    }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
  glDepthMask(GL_TRUE);
}
