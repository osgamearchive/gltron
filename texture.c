#include "gltron.h"

void deleteTextures(gDisplay *d) {
  glDeleteTextures(1, &(d->texFloor));
  glDeleteTextures(1, &(d->texWall_1));
  glDeleteTextures(1, &(d->texWall_2));
  glDeleteTextures(1, &(d->texWall_3));
  glDeleteTextures(1, &(d->texWall_4));
  glDeleteTextures(1, &(d->texGui));
  glDeleteTextures(1, &(d->texCrash));
  checkGLError("texture.c deleted textures");
}

void initTextureNames(gDisplay *d) {
  glGenTextures(1, &(d->texFloor));  
  glGenTextures(1, &(d->texGui));
  glGenTextures(1, &(d->texWall_1));
  glGenTextures(1, &(d->texWall_2));
  glGenTextures(1, &(d->texWall_3));
  glGenTextures(1, &(d->texWall_4));
  glGenTextures(1, &(d->texCrash));
}

void initTexture(gDisplay *d) {
  GLint min_filter;

  checkGLError("texture.c initTexture - start");

  initTextureNames(d);

  if(game->settings->use_mipmaps)
    min_filter = GL_LINEAR_MIPMAP_LINEAR;
  else
    min_filter = GL_LINEAR;

  /* floor texture */
  glBindTexture(GL_TEXTURE_2D, d->texFloor);
  loadTexture("gltron_floor.sgi", GL_RGB16);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  checkGLError("texture.c initTextures - floor");
  /* menu icon */

  glBindTexture(GL_TEXTURE_2D, d->texGui);
  loadTexture("gltron.sgi", GL_RGBA);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  checkGLError("texture.c initTextures - gui");

  /* wall texture 1*/

  glBindTexture(GL_TEXTURE_2D, d->texWall_1);
  loadTexture("gltron_wall_1.sgi", GL_RGB);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  
  /* wall texture 2*/

  glBindTexture(GL_TEXTURE_2D, d->texWall_2);
  loadTexture("gltron_wall_2.sgi", GL_RGB);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  
  /* wall texture 3 */

  glBindTexture(GL_TEXTURE_2D, d->texWall_3);
  loadTexture("gltron_wall_3.sgi", GL_RGB);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  
  /* wall texture 4*/

  glBindTexture(GL_TEXTURE_2D, d->texWall_4);
  loadTexture("gltron_wall_4.sgi", GL_RGB);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
 
  /* crash texture */

  glBindTexture(GL_TEXTURE_2D, d->texCrash);
  loadTexture("gltron_crash.sgi", GL_RGBA);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  checkGLError("texture.c initTextures - end");
}

