#include "model.h"

#include "GL/gl.h"
#include <stdio.h>

#ifdef POLY_COUNT
extern int polycount;
#endif

void drawMeshPart(MeshPart* meshpart, int flag) {
  int i, j;
  int type, c;
  float *normal;
  float *vertex;

  for(i = 0; i < meshpart->nFaces; i++) {
    c = *(meshpart->facesizes + i);
    type = 0;
    if(c > 4) type = GL_POLYGON;
    if(c == 4) type = GL_QUADS; 
    if(c == 3) type = GL_TRIANGLES;
    if(flag & 1) type = GL_LINE_LOOP;
    if(type != 0) {
      glBegin(type);
      for(j = 0; j < c; j++) {
	normal = meshpart->normals + 3 * (i * MODEL_FACESIZE + j);
	glNormal3fv(normal);
	vertex = meshpart->vertices + 3 * (i * MODEL_FACESIZE + j);
	glVertex3fv(vertex);
      }
      glEnd();
#ifdef POLY_COUNT
      polycount += c - 2
#endif
;
    } 
  }
}

void drawExplosionPart(MeshPart* meshpart, float radius, int flag) {
  int i, j;
  int type, c;
  float *normal;
  float *vertex;

#define EXP_VECTORS 10
  float vectors[][3] = {
    { 0.03, -0.06, -0.07 }, 
    { 0.04, 0.08, -0.03 }, 
    { 0.10, -0.04, -0.07 }, 
    { 0.06, -0.09, -0.10 }, 
    { -0.03, -0.05, 0.02 }, 
    { 0.07, 0.08, -0.00 }, 
    { 0.01, -0.04, 0.10 }, 
    { -0.01, -0.07, 0.09 }, 
    { 0.01, -0.01, -0.09 }, 
    { -0.04, 0.04, 0.02 }
  };


  for(i = 0; i < meshpart->nFaces; i++) {
    c = *(meshpart->facesizes + i);
    type = 0;
    if(c > 4) type = GL_POLYGON;
    if(c == 4) type = GL_QUADS; 
    if(c == 3) type = GL_TRIANGLES;
    if(flag & 1) type = GL_LINE_LOOP;
    if(type != 0) {
      glPushMatrix();
      normal = meshpart->normals + 3 * (i * MODEL_FACESIZE);
      glTranslatef(radius * (*(normal + 0) + vectors[i % EXP_VECTORS][0]),
		   radius * (*(normal + 1) + vectors[i % EXP_VECTORS][1]),
		   radius * (*(normal + 2) + vectors[i % EXP_VECTORS][2]) );

      glBegin(type);
      for(j = 0; j < c; j++) {
	normal = meshpart->normals + 3 * (i * MODEL_FACESIZE + j);
	glNormal3fv(normal);
	vertex = meshpart->vertices + 3 * (i * MODEL_FACESIZE + j);
	glVertex3fv(vertex);
      }
      glEnd();
#ifdef POLY_COUNT
      polycount += c - 2;
#endif
      glPopMatrix();
    } 
  }
}

void printColor(float *values, int count) {
  int i;
  printf("color: ");
  for(i = 0; i < count; i++)
    printf("%.2f", *(values + i));
  printf("\n");
}


void drawModel(Mesh *mesh, int mode, int flag) {
  int i;

  for(i = 0; i < mesh->nMaterials; i++) {
    /* set materials */
    if(mode & MODEL_USE_MATERIAL) {
      glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
		   (mesh->materials + i)->ambient);

      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
		   (mesh->materials + i)->diffuse);

      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
		   (mesh->materials + i)->specular);
    }
    drawMeshPart(mesh->meshparts + i, flag);
  }
}

void drawExplosion(Mesh *mesh, float radius, int mode, int flag) {
  int i;
  /* printf("radius: %.2f\n", radius); */
  for(i = 0; i < mesh->nMaterials; i++) {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
		   (mesh->materials + i)->ambient);

      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
		   (mesh->materials + i)->diffuse);

      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
		   (mesh->materials + i)->specular);
      drawExplosionPart(mesh->meshparts + i, radius, flag);
  }
}
