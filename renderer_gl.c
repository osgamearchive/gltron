#include <GL/gl.h>
#include <stdio.h>

void printRendererInfo() {
  printf("OpenGL Info: '%s'\n%s - %s\n", glGetString(GL_VENDOR),
	 glGetString(GL_RENDERER), glGetString(GL_VERSION));
  printf("Extensions available: %s\n", glGetString(GL_EXTENSIONS));
}
