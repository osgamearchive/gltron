#include "gltron.h"

int getTrailCount() {
  int i;
  int c = 0;
  line *l;
  for(i = 0; i < game->players; i++) {
    l = game->player[i].data->trails;
    while(l != game->player[i].data->trail) {
      l++;
      c++;
    }
  }
  /* fprintf(stderr, "trails: %d\n", c); */
  return c;
}
void bufferPlayerBow(Player *p, QuadBuffer *qb) {
  Data *data;
  Quad *q;
  float height;
  float ex, ey, sx, sy;
  int bdist;
  float white[] = { 1.0, 1.0, 1.0, 1.0 };

  data = p->data;
  height = data->trail_height;
  if(height < 0) return;

  q = getNextQuad(qb);
  q->type = QUAD_COLOR;
  glShadeModel(GL_SMOOTH);

  if(data->speed > 0 && game->settings->show_model == 1) {
    q->type |= QUAD_TEXTURE | QUAD_TEX_MODULATE;
    q->texture_id = game->screen->textures[TEX_TRAIL];
  }

  bdist = (game->settings->show_model &&
	   data->speed > 0) ? 2 : 3;

  sx = getSegmentEndX(data->trail, data, 0);
  sy = getSegmentEndY(data->trail, data, 0);

  ex = getSegmentEndX(data->trail, data, bdist);
  ey = getSegmentEndY(data->trail, data, bdist);

  q_setColor4fv(q, 0, white);
  q_setTexCoord2f(q, 0, 0.0, 0.0);
  q_setVertex3f(q, 0, sx, sy, 0.0);

  q_setColor4fv(q, 1, p->model->color_model);
  q_setTexCoord2f(q, 1, 1.0, 0.0);
  q_setVertex3f(q, 1, ex, ey, 0.0);

  q_setColor4fv(q, 2, p->model->color_model);
  q_setTexCoord2f(q, 2, 1.0, 1.0);
  q_setVertex3f(q, 2, ex, ey, height);

  q_setColor4fv(q, 3, white);
  q_setTexCoord2f(q, 3, 0.0, 1.0);
  q_setVertex3f(q, 3, sx, sy, height);
}

void bufferPlayerTrail(Player *p, QuadBuffer *qb) {
  line *line;
  float height;
  float uv, ex, ey;
  float normal1[] = { 1.0, 0.0, 0.0 };
  float normal2[] = { 0.0, 1.0, 0.0 };
  float *normal;
  float color[4];
  float white[] = { 1.0, 1.0, 1.0, 1.0 };
  Data *data;
  Quad *q;
  int tex;

  tex = game->screen->textures[TEX_DECAL];

  data = p->data;
  height = data->trail_height;

  if(height < 0) return;

  /* calculate trail color and set blending modes */
  if(game->settings->alpha_trails) {
    setColor4fv(p->model->color_alpha);
  } else {
    setColor3fv(p->model->color_alpha);
  }

  /* start drawing */
  line = &(data->trails[0]);
  while(line != data->trail) { /* the last segment is special cased */
    q = getNextQuad(qb);
    if(game->settings->softwareRendering == 0 && 
       game->settings->show_decals == 1) 
      q->type = QUAD_COLOR | QUAD_TEXTURE | QUAD_TEX_DECAL;
    else q->type = QUAD_COLOR;
    q->texture_id = tex;
    if(line->sy == line->ey) normal = normal1;
    else normal = normal2;

    /* glNormal3fv(normal); */
    setNormal3fv(normal);
    setVertex3f((line->sx + line->ex) / 2, (line->sy + line->ey) / 2, 0);
    light4fv(color);
    
    q_setColor4fv(q, 0, color);
    q_setTexCoord2f(q, 0, 0.0, 0.0);
    q_setVertex3f(q, 0, line->sx, line->sy, 0.0);

    uv = getSegmentUV(line);

    q_setColor4fv(q, 1, color);
    q_setTexCoord2f(q, 1, uv, 0.0);
    q_setVertex3f(q, 1, line->ex, line->ey, 0.0);

    q_setColor4fv(q, 2, color);
    q_setTexCoord2f(q, 2, uv, 1.0);
    q_setVertex3f(q, 2, line->ex, line->ey, height);

    q_setColor4fv(q, 3, color);
    q_setTexCoord2f(q, 3, 0.0, 1.0);
    q_setVertex3f(q, 3, line->sx, line->sy, height);


    line++;
  }

  if(line->sy == data->posy) normal = normal1;
  else normal = normal2;
  /* glNormal3fv(normal); */

  /* calculate segment color */
  setNormal3fv(normal);
  setVertex3f(line->sx, line->sy, 0);
  light4fv(color);

  q = getNextQuad(qb);
  if(game->settings->softwareRendering == 0 && 
     game->settings->show_decals == 1) 
    q->type = QUAD_COLOR | QUAD_TEXTURE | QUAD_TEX_DECAL;
  else q->type = QUAD_COLOR;

  q->texture_id = tex;

  q_setColor4fv(q, 0, color);
  q_setTexCoord2f(q, 0, 0.0, 0.0);
  q_setVertex3f(q, 0, line->sx, line->sy, 0.0);

  uv = getSegmentEndUV(line, data);
  ex = getSegmentEndX(line, data, 1);
  ey = getSegmentEndY(line, data, 1);

  q_setColor4fv(q, 1, color);
  q_setTexCoord2f(q, 1, uv, 0.0);
  q_setVertex3f(q, 1, ex, ey, 0.0);

  /* uv = getSegmentUV(line); // wrong! */
  q_setColor4fv(q, 2, color);
  q_setTexCoord2f(q, 2, uv, 1.0);
  q_setVertex3f(q, 2, ex, ey, height);


  q_setColor4fv(q, 3, color);
  q_setTexCoord2f(q, 3, 0.0, 1.0);
  q_setVertex3f(q, 3, line->sx, line->sy, height);

  /* 
  printf("uv for last segment: %.3f\n");
  printf("segment lenght: %.2f\n", (ex - line->sx) + (ey - line->sy));
  */

  /* experimental trail effect */
  checkGLError("before trail");

  q = getNextQuad(qb);
  q->type = QUAD_COLOR;
    
  q_setColor4fv(q, 0, color);
  q_setVertex3f(q, 0, ex, ey, 0.0);

  q_setColor4fv(q, 3, color);
  q_setVertex3f(q, 3, ex, ey, height);

  uv = getSegmentUV(line);
  ex = getSegmentEndX(line, data, 0);
  ey = getSegmentEndY(line, data, 0);

  memcpy(color, white, sizeof(color));

  q_setColor4fv(q, 2, color);
  q_setVertex3f(q, 2, ex, ey, height);

  q_setColor4fv(q, 1, color);
  q_setVertex3f(q, 1, ex, ey, 0.0);

}

void drawTrails(QuadBuffer *q, int *index) {
  int i;

  if(index == NULL) {
    for(i = 0; i < q->current; i++)
      renderQuad(q->quads + i);
  } else {
    for(i = 0; i < q->current; i++) {
      /* printf("drawing quad %d\n", index[i]); */
      renderQuad(q->quads + index[i] );
    }
  }
  glDisable(GL_TEXTURE_2D);
}
 
void doTrails(Player *p) {
  static QuadBuffer *q = NULL;
  int size;
  int *index;
  if(q == NULL) {
    size = getTrailCount() + 12;
    printf("allocating QuadBuffer, size %d\n", size);
    q = createQuadBuffer(size);
  } else {
    size = getTrailCount() + 12;
    if(size > q->size) { 
      printf("reallocating QuadBuffer, size %d\n", size);
      freeQuadBuffer(q);
      /* we don't want to reallocate immediately, no make it a bit larger */
      q = createQuadBuffer(size + 36); 
    }
  }
  q->current = 0;
  clearState();
  if(game->settings->alpha_trails) {
    /* depth sort everything */
    int i;
    for(i = 0; i < game->players; i++) {
      bufferPlayerTrail(game->player + i, q);
      bufferPlayerBow(game->player + i, q);
    }
    index = getSortedQuads(q, p->camera->cam);
    drawTrails(q, index);
    if(index != NULL) free(index);
  } else {
    /* draw non-transparent trails first (unsorted), then draw
       bows */
    int i;
    for(i = 0; i < game->players; i++)
      bufferPlayerTrail(game->player + i, q);
    drawTrails(q, NULL);
    for(i = 0; i < game->players; i++)
      bufferPlayerBow(game->player + i, q);
    /* bows are transparent, so sort back-to-front */
    index = getSortedQuads(q, p->camera->cam);
    drawTrails(q, index);
    if(index != NULL) free(index);
  }
  
  /* 
     printf("%d texture bounds\n", state->binds);
     printf("%d texture mod changes\n", state->mod_changes);
  */
}


