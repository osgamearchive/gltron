#ifndef SKYBOX_H
#define SKYBOX_H

class SkyBox {
   
  public :
  static void enableTexture();
  static void disableTexture();
  static void bindTexture(int index);
  static void draw();
};

#endif /* SKYBOX_H */


