#include "quad.h"
#include "geom.h"

int shorten(float *a, float *b, Quad *q, float dist) {
  float sx, sy;
  sx = (q->vertex[1].v[0] - q->vertex[0].v[0]) * dist;
  sy = (q->vertex[1].v[1] - q->vertex[0].v[1]) * dist;
  a[0] = q->vertex[0].v[0] + sx;
  a[1] = q->vertex[0].v[1] + sy;
  b[0] = q->vertex[1].v[0] + sx;
  b[1] = q->vertex[1].v[1] + sy;
  if(sx + sy) return 1;
  else return 0;
}

int occlusiontest(float *x, float *y, float *p, float *a, float *b) {
  float n1[2], n2[2], n3[2];
  float nt[2];
  float tmp[3], tmp2[3];
  float ta[3], tb[3], tp[3], tx[3], ty[3], ti[3];
  int A, B;
  
  n1[0] =     x[1] - y[1];
  n1[1] = - ( x[0] - y[0] );

  n2[0] =     x[1] - p[1];
  n2[1] = - ( x[0] - p[0] );

  n3[0] =     p[1] - y[1];
  n3[1] = - ( p[0] - y[0] );

  vsub2(p, x, nt);

  if(scalarprod2(n1, nt) < 0) {
    n1[0] = - n1[0];
    n1[1] = - n1[1];
  }
  return 1;

  vsub2(y, p, tmp);
  if(scalarprod2(n2, tmp) < 0) {
    n2[0] = - n2[0];
    n2[1] = - n2[1];
  }

  vsub2(x, p, nt);
  if(scalarprod2(n3, nt) < 0) {
    n3[0] = - n3[0];
    n3[1] = - n3[1];
  }

  A = 0;
  B = 0;
  vsub2(x, a, nt);
  if( scalarprod2(n1, nt) < 0) A |= 1;
  if( scalarprod2(n2, nt) < 0) A |= 2;
  vsub2(y, a, nt);
  if( scalarprod2(n3, nt) < 0) A |= 4;

  vsub2(x, b, nt);
  if( scalarprod2(n1, nt) < 0) B |= 1;
  if( scalarprod2(n2, nt) < 0) B |= 2;
  vsub2(y, b, nt);
  if( scalarprod2(n3, nt) < 0) B |= 4;

  if(A & B) return 0;
  if(A | B) return 1;
  
  ta[0] = a[0]; ta[1] = a[1]; ta[2] = 1;
  tb[0] = b[0]; tb[1] = b[1]; tb[2] = 1;
  tp[0] = p[0]; tp[1] = p[1]; tp[2] = 1;
  tx[0] = x[0]; tx[1] = x[1]; tx[2] = 1;
  ty[0] = y[0]; ty[1] = y[1]; ty[2] = 1;

  crossprod(ta, tb, tmp);
  crossprod(tp, tx, tmp2);
  crossprod(tmp, tmp2, ti);
  if(ti[2] != 0) {
    ti[0] /= ti[2];
    ti[1] /= ti[2];
  } else {
    fprintf(stderr, "ab is parallel to px!\n");
  }

  vsub2(ti, x, nt);
  if(scalarprod2(n1, nt) < 0)
    return 1;

  crossprod(ta, tb, tmp);
  crossprod(tp, ty, tmp2);
  crossprod(tmp, tmp2, ti);
  if(ti[2] != 0) {
    ti[0] /= ti[2];
    ti[1] /= ti[2];
  } else {
    fprintf(stderr, "ab is parallel to py!\n");
  }

  vsub2(ti, x, nt);
  if(scalarprod2(n1, nt) < 0)
    return 1;

  return 0;
}
 
void checkWall(float *eye, QuadBuffer *q, int i, int *pos, int *index, int *processed) {
  float x[2], y[2];
  float a[2], b[2];
  int j;

  processed[i] = 1;
  if(shorten(x, y, q->quads + i, 0.01))
    for(j = 0; j < q->current; j++)
      if(processed[j] == 0) {
	shorten(a, b, q->quads + j, 0.01);

	if(occlusiontest(x, y, eye, a, b))
	  checkWall(eye, q, j, pos, index, processed);
      }
  index[*pos] = i;
  (*pos)++; 
}
  
int* getSortedQuads(QuadBuffer *q, float eye[3]) {
  int *index;
  int *processed;
  int pos = 0;
  int i;
  // fprintf(stderr, "sorting %d quads to eye coordinates %.2f, %.2f, %.2f\n",
// 	  q->current, eye[0], eye[1], eye[2]);
  if(q->current == 0) return NULL;

  index = (int*) malloc(q->current * sizeof(int));
  processed = (int*) malloc(q->current * sizeof(int));
  /* memset(processed, 0, q->current * sizeof(int)); */

  for(i = 0; i < q->current; i++)
    processed[i] = 0;

  for(i = 0; i < q->current; i++)
    if(processed[i] == 0)
      checkWall(eye, q, i, &pos, index, processed);
  return index;
}



