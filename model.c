#include "model.h"
#include "geom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_V 30000
#define MAX_F 20000
#define MAX_N 30000

#define FAIL(X) free(vert); free(face); free(norm); free(normi); \
                printf(X); return NULL;

void rescaleVertices(float *vertices, float size, int nVertices, float *bbox) {
  float x, y, z, xmax, ymax, zmax, max;
  float *f;
  int i;

  /* printf("rescaling %d vertices\n", nVertices); */

  if(nVertices == 0) return;
  x = *(vertices);
  y = *(vertices + 1);
  z = *(vertices + 2);

  for(i = 0; i < nVertices; i++) {
    f = vertices + 3 * i;
    if(*f < x)
      x = *f;
    if(*(f + 1) < y)
      y = *(f + 1);
    if(*(f + 2) < z)
      z = *(f + 2);
  }
  xmax = 0; ymax = 0; zmax = 0;
  for(i = 0;  i < nVertices; i ++) {
    f = vertices + 3 * i;
    *f = *f - x;
    *(f + 1) = *(f + 1) - y;
    *(f + 2) = *(f + 2) - z;
    if(*f > xmax) xmax = *f;
    if(*(f + 1) > ymax) ymax = *(f + 1);
    if(*(f + 2) > zmax) zmax = *(f + 2);
  }
  if(xmax + ymax + zmax == 0) return;

  if (xmax > ymax)
    max = (xmax > zmax) ? xmax : zmax;
  else
    max = (ymax > zmax) ? ymax : zmax;

  for(i = 0;  i < nVertices; i ++) {
    f = vertices + 3 * i;
    *f = *f / max * size;
    *(f + 1) = *(f + 1) / max * size;
    *(f + 2) = *(f + 2) / max * size;
  }
  
  bbox[0] = xmax / max * size;
  bbox[1] = ymax / max * size;
  bbox[2] = zmax / max * size;


  printf("translated model by (%f, %f, %f)\n", x, y, z);
  printf("scaled model down by %f\n", max * size);
  printf("bounding box: (0, 0, 0) - (%.2f, %.2f, %.2f)\n",
	 bbox[0], bbox[1], bbox[2]);

}
  
Mesh* loadModel(const char *filename, float size, int flags) {
  /* faces: only quads or triangles at the moment */
  Mesh* mesh;
  Material *materials;

  FILE *f;
  char buf[120];
  char namebuf[120];
  char *path;

  float *vert;
  int *face;
  float *norm;
  int *normi;
  int *matIndex;
  
  float *vertex, *normal;

  int nNormals = 0;
  int nVertices = 0;
  int nFaces = 0;

  float *meshVerts;
  float *meshNorms;
  int *meshFacesize;
  int currentMat = 0;
  int matCount = 0;
  int *pMatCount = NULL;
  int iLine = 0;

  float t1[3], t2[3], t3[3];
  int c, i, j, k, l, pos;
  int hasNorms = 0;
  int hasTexture = 0;
  int inv;

  if((f = fopen(filename, "r")) == 0) {
    fprintf(stderr, "could not open file\n");
    return NULL;
  }

  vert = (float *) malloc(sizeof(float) * 3 * MAX_V);
  face = (int *) malloc(sizeof(int) * MODEL_FACESIZE * MAX_F);
  matIndex = (int *) malloc(sizeof(int) * MAX_F);
  normi = (int *) malloc(sizeof(int) * MODEL_FACESIZE * MAX_F);
  norm = (float *) malloc(sizeof(float) * 3 * MAX_N);
  
  while(fgets(buf, sizeof(buf), f)) {
    switch(buf[0]) {
    case 'm': /* material library? */
      if(sscanf(buf, "mtllib %s ", namebuf) == 1) {
	/* load material library */
	path = getFullPath(namebuf);
	if(path == NULL) {
	  fprintf(stderr, "fatal: can't find mtllib '%s'\n", namebuf);
	  exit(1);
	}
	matCount = loadMaterials(path, &materials);
	if(matCount <= 0) {
	  fprintf(stderr, "fatal: no Materials loaded\n");
	  exit(1);
	}
	/* printf("loaded %d Materials\n", matCount); */
	pMatCount = (int*) malloc(sizeof(int) * matCount);
	for(i = 0; i < matCount; i++)
	  pMatCount[i] = 0;
	currentMat = 0;
      } else
	fprintf(stderr, "warning: ignored line %d\n", iLine);
      break;
    case 'u': case 'g': /* material name */
      if(sscanf(buf, "usemtl %s ", namebuf) == 1) {
	for(i = 0; i < matCount; i++) {
	  if(strcmp(namebuf, (materials + i)->name) == 0) {
	    currentMat = i;
	    break; /* break out of if */
	  }
	}
      } else currentMat = 0;
      break;
    case 'v':
      switch(buf[1]) {
      case ' ': /* vertex data */
	if(nVertices >= MAX_V) {
	  FAIL("vertex limit exceeded\n") ;
	}
	c = sscanf(buf, "v %f %f %f ",
		   vert + nVertices * 3,
		   vert + nVertices * 3 + 1,
		   vert + nVertices * 3 + 2);

	for(i = c; i < 3; i++) {
	  printf("this should not happen\n");
	  *(vert + nVertices * 3 + i) = 0;
	}
	nVertices++;
	break;
      case 'n': /* vertex normal */
	hasNorms = 1;
	if(nVertices >= MAX_N) {
	  FAIL("normals limit exceeded\n") ;
	}
	c = sscanf(buf, "vn %f %f %f ", 
		   norm + nNormals * 3,
		   norm + nNormals * 3 + 1,
		   norm + nNormals * 3 + 2);
	for(i = c; i < 3; i++) {
	  printf("this should not happen\n");
	  *(norm + nNormals * 3 + i) = 0;
	  break;
	}
	nNormals++;
	break;
      case 't': /* texture coordinate - ignored */
	hasTexture = 1;
	break;
      }
      break;
    case 'f':
      if(nFaces * MODEL_FACESIZE >= MAX_F) {
	FAIL("face limit exceeded\n") ;
      }
      /* mark material */
      *(matIndex + nFaces) = currentMat;
      if(matCount > 0 && pMatCount != NULL)
	pMatCount[currentMat]++;

      if(hasNorms) {
	int dummy1, dummy2, dummy3, dummy4;
	if (hasTexture) {
	  c = sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d ",
		     face + nFaces * MODEL_FACESIZE, &dummy1,
		     normi + nFaces * MODEL_FACESIZE,
		     face + nFaces * MODEL_FACESIZE + 1, &dummy2,
		     normi + nFaces * MODEL_FACESIZE + 1,
		     face + nFaces * MODEL_FACESIZE + 2, &dummy3,
		     normi + nFaces * MODEL_FACESIZE + 2,
		     face + nFaces * MODEL_FACESIZE + 3, & dummy4,
		     normi + nFaces * MODEL_FACESIZE + 3); 
	} else {
	  c = sscanf(buf, "f %d//%d %d//%d %d//%d %d//%d ",
		     face + nFaces * MODEL_FACESIZE,
		     normi + nFaces * MODEL_FACESIZE,
		     face + nFaces * MODEL_FACESIZE + 1,
		     normi + nFaces * MODEL_FACESIZE + 1,
		     face + nFaces * MODEL_FACESIZE + 2,
		     normi + nFaces * MODEL_FACESIZE + 2,
		     face + nFaces * MODEL_FACESIZE + 3,
		     normi + nFaces * MODEL_FACESIZE + 3);
	}

	for(i = c / 2; i < MODEL_FACESIZE; i++) {
	  *(face + nFaces * MODEL_FACESIZE + i) = -1;
	  *(normi + nFaces * MODEL_FACESIZE + i) = -1;
	}

	nFaces++;
      } else {
	/* TODO: add if(hasTexture) */
	c = sscanf(buf, "f %d %d %d %d ",
		   face + nFaces * MODEL_FACESIZE,
		   face + nFaces * MODEL_FACESIZE + 1,
		   face + nFaces * MODEL_FACESIZE + 2,
		   face + nFaces * MODEL_FACESIZE + 3);
	for(i = c; i < MODEL_FACESIZE; i++)
	  *(face + nFaces * MODEL_FACESIZE + i) = -1;
	nFaces++;
      }
      break;
    }
    iLine++;
  }
  if(hasNorms == 0) {
    /* create Normals */
    for(i = 0; i < nFaces; i++) {
      t1[0] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 0) - 1) + 0);
      t1[1] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 0) - 1) + 1);
      t1[2] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 0) - 1) + 2);

      t2[0] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 1) - 1) + 0);
      t2[1] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 1) - 1) + 1);
      t2[2] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 1) - 1) + 2);

      t3[0] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 2) - 1) + 0);
      t3[1] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 2) - 1) + 1);
      t3[2] = *(vert + 3 * (*(face + i * MODEL_FACESIZE + 2) - 1) + 2);
      /*
      printf("face %d:\n", i);
      printf("v1: %f %f %f\n", t1[0], t1[1], t1[2]);
      printf("v2: %f %f %f\n", t2[0], t2[1], t2[1]);
      printf("v3: %f %f %f\n", t3[0], t3[1], t3[1]);
      */
      t1[0] -= t3[0];
      t1[1] -= t3[1];
      t1[2] -= t3[2];
      t2[0] -= t3[0];
      t2[1] -= t3[1];
      t2[2] -= t3[2];
      normcrossprod(t1, t2, t3);
      /* printf("normal: %f %f %f\n\n", t3[0], t3[1], t3[2]); */
      /* t3 now contains the face normal */
      for(j = 0; j < MODEL_FACESIZE; j++) 
	*(normi + i * MODEL_FACESIZE + j) = nNormals + 1;
      *(norm + nNormals * 3 + 0) = t3[0];
      *(norm + nNormals * 3 + 1) = t3[1];
      *(norm + nNormals * 3 + 2) = t3[2];
      nNormals++;
    }
    /* printf("generated %d normals\n", nNormals); */
  }


  if(matCount == 0) { /* create Default material */
    float spec[] = { 0.77, 0.77, 0.77, 1.0 };
    float dif[] = { 0.4, 0.4, 0.4, 1};
    float amb[] = { 0.25, 0.25, 0.25, 1};

    materials = (Material*) malloc(sizeof(Material));
    materials->name = (char*) malloc(strlen("default") + 1);
    sprintf(materials->name, "default");
    
    memcpy(materials->ambient, amb, 3 * sizeof(float));
    memcpy(materials->diffuse, dif, 3 * sizeof(float));
    memcpy(materials->specular, spec, 3 * sizeof(float));

    matCount = 1;
    pMatCount = (int*) malloc(sizeof(int));
    pMatCount[0] = nFaces;
  }
  /* everything is parsed, now allocate memory and */
  /* rescale and get bbox */
  /* copy data to Mesh structure */
  /* new: sort into sub meshes with by materials */

  if(flags & MODEL_INVERT_NORMALS) { /* invert normals */
    /* printf("inverting normals...really!\n"); */
    inv = -1;
  } else inv = 1;

  mesh = (Mesh*) malloc(sizeof(Mesh));

  /* rescale */

  rescaleVertices(vert, size, nVertices, mesh->bbox);

  mesh->nFaces = nFaces;
  mesh->nMaterials = matCount;
  mesh->materials = materials;
  mesh->meshparts = (MeshPart*) malloc(matCount * sizeof(MeshPart));
  for(i = 0; i < matCount; i++) {
    meshVerts = (float*) malloc(pMatCount[i] * 3 * MODEL_FACESIZE * sizeof(float));
    meshNorms = (float*) malloc(pMatCount[i] * 3 * MODEL_FACESIZE * sizeof(float));
    meshFacesize = (int*) malloc(pMatCount[i] * sizeof(int));

    /* printf("Material %d: %d faces\n", i, pMatCount[i]); */

    (mesh->meshparts + i)->nFaces = pMatCount[i];
    (mesh->meshparts + i)->vertices = meshVerts;
    (mesh->meshparts + i)->normals = meshNorms;
    (mesh->meshparts + i)->facesizes = meshFacesize;
    pos = 0;
    for(j = 0; j < nFaces; j++) { /* foreach face */
      if(matIndex[j] == i) {
	*(meshFacesize + pos) = 0;
	/* printf("face %d\n", j); */
	for(k = 0; k < MODEL_FACESIZE; k++) { /* foreach vertex of face */
	  if(*(face + j * MODEL_FACESIZE + k) != -1) {
	    *(meshFacesize + pos) += 1;
	    /* adjust facesize... */
	    /* copy face and normal data to meshVerts, meshNorms */
	    vertex = vert + 3 * ( *(face + j * MODEL_FACESIZE + k) - 1);
	    normal = norm + 3 * ( *(normi + j * MODEL_FACESIZE + k) - 1);
	    if(flags & MODEL_NORMALIZE)
	      normalize(normal);

	    for(l = 0; l < 3; l++) {
	      /* printf("%f ", vertex[l]); */
	      *(meshVerts + 3 * (pos * MODEL_FACESIZE + k) + l) = *(vertex + l);
	      *(meshNorms + 3 * (pos * MODEL_FACESIZE + k) + l) = inv * *(normal + l);
	    }
	    /* printf("\n"); */
	  }

	}
	pos++;
	if(pos > pMatCount[i]) {
	  fprintf(stderr, "fatal: more faces than accounted for\n");
	  exit(1);
	}
      }
    }
  }
  
  free(vert);
  free(face);
  free(norm);
  free(normi);
  free(pMatCount);
  free(matIndex);

  fclose (f);
  /* printf("loaded model: %d vertices, %d normals, %d faces, %d materials\n",
	nVertices, nNormals, nFaces, matCount); */

  return mesh;
}

void setMaterialAmbient(Mesh *mesh, int material, float color[4]) {
  int i;
  for(i = 0; i < 4; i++)
    (mesh->materials + material)->ambient[i] = color[i];
}

void setMaterialDiffuse(Mesh *mesh, int material, float color[4]) {
  int i;
  for(i = 0; i < 4; i++)
    (mesh->materials + material)->diffuse[i] = color[i];
}

void setMaterialSpecular(Mesh *mesh, int material, float color[4]) {
  int i;
  for(i = 0; i < 4; i++)
    (mesh->materials + material)->specular[i] = color[i];
}
  
void setMaterialAlphas(Mesh *mesh, float alpha) {
  int i;
  /* vertex alpha is the alpha of the diffuse material component */
  for(i = 0; i < mesh->nMaterials; i++)
    (mesh->materials + i)->diffuse[3] = alpha;
}

void unloadModel(Mesh *mesh) {
  int i;
  for(i = 0; i < mesh->nMaterials; i++) {
    /* free material */
    free( (mesh->materials + i)->name );
    free( (mesh->materials + i) );
    /* free meshpart */
    free( (mesh->meshparts + i)->facesizes );
    free( (mesh->meshparts + i)->vertices );
    free( (mesh->meshparts + i)->normals );
    free( (mesh->meshparts + i) );
  }
}
  
