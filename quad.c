#include "quad.h"

/* see quad.h for more information */

void q_setColor4fv( Quad *q, int index, float color[4]) {
  memcpy(q->vertex[index].color, color, 4 * sizeof(float));
}

void q_setColor4f( Quad *q, int index, float r, float g, float b, float a) {
  q->vertex[index].color[0] = r;
  q->vertex[index].color[1] = g;
  q->vertex[index].color[2] = b;
  q->vertex[index].color[3] = a;
}

void q_setColor3f( Quad *q, int index, float r, float g, float b) {
  q_setColor4f( q, index, r, g, b, 1.0);
}

void q_setTexCoord2f( Quad *q, int index, float u, float v ) {
  q->vertex[index].uv[0] = u;
  q->vertex[index].uv[1] = v;
}

void q_setVertex3f( Quad *q, int index, float x, float y, float z) {
  q->vertex[index].v[0] = x;
  q->vertex[index].v[1] = y;
  q->vertex[index].v[2] = z;
}

QuadBuffer* createQuadBuffer(int size) {
  QuadBuffer *q;
  q = (QuadBuffer*) malloc(sizeof(struct QuadBuffer));
  if(q != NULL) {
    q->quads = (Quad*) malloc(sizeof(struct Quad) * size);
    if(q->quads != 0) {
      q->size = size;
      q->current = 0;
      return q;
    } else free(q);
  }
  fprintf(stderr, "failed to allocate quad buffer\n");
  exit(2);
}

void freeQuadBuffer(QuadBuffer *q) {
  free(q->quads);
  free(q);
}

extern Quad *getNextQuad(QuadBuffer *q) {
  if(q->current < q->size)
    return q->quads + q->current++;
  else {
    fprintf(stderr, "quad buffer overflow!\n");
    return NULL;
  }
}

int getCurrentQuad(QuadBuffer *q) {
  return q->current;
}


int getQuadBufferSize(QuadBuffer *q) {
  return q->size;
}

void q_setType(Quad *q, int type) {
  q->type = type;
}

Quad* getQuadAt(QuadBuffer *q, int index) {
  return q->quads + index;
}

void renderQuadAt(QuadBuffer *q, int* index, int i) {
  renderQuad(q->quads + index[i]);
}
