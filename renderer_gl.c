#include "renderer_gl.h"

GLstate rendererState;
GLstate *state = &rendererState;

void clearState() {
  state->tex_id = 0;
  state->tex_env_mode = 0;
  state->binds = 0;
  state->mod_changes = 0;
}

void printRendererInfo() {
  printf("OpenGL Info: '%s'\n%s - %s\n", glGetString(GL_VENDOR),
	 glGetString(GL_RENDERER), glGetString(GL_VERSION));
  printf("Extensions available: %s\n", glGetString(GL_EXTENSIONS));
}

void renderQuad(Quad *q) {
  int i;
  if(q->type & QUAD_TEXTURE) {
    glEnable(GL_TEXTURE_2D);
    if(state->tex_id != q->texture_id) {
      glBindTexture(GL_TEXTURE_2D, q->texture_id);
      state->binds++;
      state->tex_id = q->texture_id;
    }
    if(q->type & QUAD_TEX_DECAL) {
      if(state->tex_env_mode != GL_DECAL) {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	state->tex_env_mode = GL_DECAL;
	state->mod_changes++;
      }
    } else if(q->type & QUAD_TEX_MODULATE)
      if(state->tex_env_mode != GL_MODULATE) {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	state->tex_env_mode = GL_MODULATE;
	state->mod_changes++;
      }
  } else {
    glDisable(GL_TEXTURE_2D);
  }

  /* fprintf(stderr, "drawing quad...errors: %d\n", glGetError()); */

  glBegin(GL_QUADS);
  
  for(i = 0; i < 4; i++) {
    if(q->type & QUAD_COLOR) glColor4fv(q->vertex[i].color);
    if(q->type & QUAD_TEXTURE) glTexCoord2fv(q->vertex[i].uv);
    glVertex3fv(q->vertex[i].v);
    // printVector3f(q->vertex[i].v);
  }
  glEnd();
}

