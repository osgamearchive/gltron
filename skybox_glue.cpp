#include "SkyBox.h"

extern "C" {

#include "skybox_glue.h"

void SkyBox_draw(void) {
  SkyBox::draw();
}

void SkyBox_enableTexture(void) {
  SkyBox::enableTexture();
}

void SkyBox_disableTexture(void) {
  SkyBox::disableTexture();
}

}

