#ifndef SKYBOX_H
#define SKYBOX_H

#include "GL/gl.h"

void enableSkyboxTexture();
void disableSkyboxTexture();
void bindSkyboxTexture(int index);
GLuint buildSkyboxDispList(GLuint skybox_list);

#endif /* SKYBOX_H */


