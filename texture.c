#include "gltron.h"

void deleteTextures(gDisplay *d) {
  glDeleteTextures(1, &(d->texFloor));
  glDeleteTextures(1, &(d->texWall_1));
  glDeleteTextures(1, &(d->texWall_2));
  glDeleteTextures(1, &(d->texWall_3));
  glDeleteTextures(1, &(d->texWall_4));
  glDeleteTextures(1, &(d->texGui));
  glDeleteTextures(1, &(d->texLogo));
  glDeleteTextures(1, &(d->texCrash));
  glDeleteTextures(1, &(d->texTrail));
  glDeleteTextures(1, &(d->texTrailDecal));

  checkGLError("texture.c deleted textures");
}

void initTextureNames(gDisplay *d) {
  glGenTextures(1, &(d->texFloor));  
  glGenTextures(1, &(d->texGui));
  glGenTextures(1, &(d->texLogo));
  glGenTextures(1, &(d->texWall_1));
  glGenTextures(1, &(d->texWall_2));
  glGenTextures(1, &(d->texWall_3));
  glGenTextures(1, &(d->texWall_4));
  glGenTextures(1, &(d->texCrash));
  glGenTextures(1, &(d->texTrail));
  glGenTextures(1, &(d->texTrailDecal));
  checkGLError("texture.c allocated texture names");
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
  loadTexture("gltron_floor" TEX_SUFFIX, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  checkGLError("texture.c initTextures - floor");

  /* menu background */

  glBindTexture(GL_TEXTURE_2D, d->texGui);
  loadTexture("gltron" TEX_SUFFIX, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  checkGLError("texture.c initTextures - gui");

  /* menu logo */

  glBindTexture(GL_TEXTURE_2D, d->texLogo);
  loadTexture("gltron_logo" TEX_SUFFIX, GL_RGBA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  checkGLError("texture.c initTextures - logo");

  /* wall texture 1*/

  glBindTexture(GL_TEXTURE_2D, d->texWall_1);
  loadTexture("gltron_wall_1" TEX_SUFFIX, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  
  /* wall texture 2*/

  glBindTexture(GL_TEXTURE_2D, d->texWall_2);
  loadTexture("gltron_wall_2" TEX_SUFFIX, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  
  /* wall texture 3 */

  glBindTexture(GL_TEXTURE_2D, d->texWall_3);
  loadTexture("gltron_wall_3" TEX_SUFFIX, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  
  /* wall texture 4*/

  glBindTexture(GL_TEXTURE_2D, d->texWall_4);
  loadTexture("gltron_wall_4" TEX_SUFFIX, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
 
  /* crash texture */

  glBindTexture(GL_TEXTURE_2D, d->texCrash);
  loadTexture("gltron_crash" TEX_SUFFIX, GL_RGBA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  /* trail bow texture */

  glBindTexture(GL_TEXTURE_2D, d->texTrail);
  loadTexture("gltron_trail" TEX_SUFFIX, GL_RGBA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  /* trail decal texture */

  glBindTexture(GL_TEXTURE_2D, d->texTrailDecal);
  loadTexture("gltron_traildecal" TEX_SUFFIX, GL_RGBA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

  checkGLError("texture.c initTextures - end");
}

