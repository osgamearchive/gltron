#ifndef SKYBOX_H
#define SKYBOX_H
   
void enableSkyboxTexture();
void disableSkyboxTexture();
void bindSkyboxTexture(int index);
void buildSkyboxDispList(GLuint skybox_list);

#endif /* SKYBOX_H */


