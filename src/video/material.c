#include "video/model.h"
#include "filesystem/path.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zlib.h"

void loadDefaultMaterial(Material *pMaterial) {
  float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0 };
  float diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
  float specular[] = { 1.0, 1.0, 1.0, 1.0 };
  float shininess = 2;

  memcpy( pMaterial->ambient, ambient, sizeof(ambient) );
  memcpy( pMaterial->diffuse, diffuse, sizeof(diffuse) );
  memcpy( pMaterial->specular, specular, sizeof(specular) );
  pMaterial->shininess = shininess;
  pMaterial->name = NULL;
  pMaterial->map_diffuse = NULL;
}

void readMaterialLibraryFromFile(char *filename, Mesh *pMesh) {
  Material *pMaterials; 
  int iMaterial;

  char buf[BUF_SIZE];
  char name[BUF_SIZE];
  char *path;
  gzFile f;

  int i;

  path = getPath(PATH_DATA, filename);
  if(path == NULL) {
    fprintf(stderr, "** could not optain path to file '%s'\n", filename);
    return;
  }

  if((f = gzopen(path, "r")) == 0) {
    fprintf(stderr, "** could not open file '%s'\n", filename);
    free(path);
    return;
  }
  free(path);

  pMaterials = malloc( MAX_MATERIALS * sizeof( Material ) );
  iMaterial = -1;

  while( gzgets(f, buf, sizeof(buf)) ) {
    switch(buf[0]) {
    case '#': break;
    case 'n':
      iMaterial++;
      loadDefaultMaterial(pMaterials + iMaterial);

      sscanf(buf, "newmtl %s ", name);
      pMaterials[iMaterial].name = malloc( strlen(name) + 1 );
      strcpy( pMaterials[iMaterial].name, name );

      break;
    default:
      if(sscanf(buf, " Ka %f %f %f ", 
		pMaterials[iMaterial].ambient + 0,
		pMaterials[iMaterial].ambient + 1,
		pMaterials[iMaterial].ambient + 2) == 3) {
	// nothing
      } else if(sscanf(buf, " Kd %f %f %f ", 
		pMaterials[iMaterial].diffuse + 0,
		pMaterials[iMaterial].diffuse + 1,
		pMaterials[iMaterial].diffuse + 2) == 3) {
	// nothing
      } else if(sscanf(buf, " Ks %f %f %f ", 
		pMaterials[iMaterial].specular + 0,
		pMaterials[iMaterial].specular + 1,
		pMaterials[iMaterial].specular + 2) == 3) {
	// nothing
      } else if(sscanf(buf, " Ns %f ", 
		       & (pMaterials[iMaterial].shininess) ) == 1 ) {
	// nothing
      } else {
	/* FIXME: "disabled error message" */
	/* fprintf(stderr, "+++ unparsed material property: %s", buf); */
      }
    }
  }
	
  pMesh->nMaterials = iMaterial + 1;
  pMesh->pMaterials = malloc( sizeof(Material) *  pMesh->nMaterials );
  for(i = 0; i < pMesh->nMaterials; i++) {
    memcpy( pMesh->pMaterials + i, pMaterials + i, sizeof(Material) );
  }

  free(pMaterials);
}

void readMaterialLibrary(char *buf, Mesh *pMesh) {
  char filename[BUF_SIZE];
  if(sscanf(buf, " mtllib %s ", filename) != 1) {
    fprintf(stderr, "*** failing parsing filename from %s\n", buf);
    return;
  }
  readMaterialLibraryFromFile(filename, pMesh);
}

void setMaterial(char *buf, Mesh *pMesh, int *iGroup) {
  char name[BUF_SIZE];
  int i;

  if(sscanf(buf, " usemtl %s ", name) == 1) {
    // search through all materials for the current one
    for(i = 0; i < pMesh->nMaterials; i++) {
      if(strstr(pMesh->pMaterials[i].name, name) ==
	 pMesh->pMaterials[i].name) {
	*iGroup = i;
	return;
      }
    }
    fprintf(stderr, "*** error: material %s not found\n", name);
  } else {
    fprintf(stderr, "*** error: can't parse material line %s\n", buf);
  }
}

void SetMaterialColor(Mesh *pMesh, char *name, ColorType eType,
		      float pColor[4]) {
  int i;

  for(i = 0; i < pMesh->nMaterials; i++) {
    if(name == NULL || strstr(pMesh->pMaterials[i].name, name) ==
       pMesh->pMaterials[i].name) {
      switch(eType) {
      case eAmbient:
	memcpy( pMesh->pMaterials[i].ambient, pColor, sizeof(float[4]) );
	break;
      case eDiffuse:
	memcpy( pMesh->pMaterials[i].diffuse, pColor, sizeof(float[4]) );
	break;
      case eSpecular:
	memcpy( pMesh->pMaterials[i].specular, pColor, sizeof(float[4]) );
	break;
      }
    }
  }
}

/*
void copyMaterialColors(Material *pIn, Material *pOut) {
  memcpy(pOut->ambient, pIn->ambient, sizeof( ambient ));
  memcpy(pOut->diffuse, pIn->diffuse, sizeof( diffuse ));
  memcpy(pOut->specular, pIn->specular, sizeof( specular ));
  pOut->shininess = pIn->shininess;
}
*/

