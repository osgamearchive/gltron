#define MAX_MATERIALS 100

#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loadMaterials(char *filename, Material **materials) {
  Material *m;
  FILE *f;
  char buf[120];
  char namebuf[120];
  int iMaterial = -1;
  int iLine = 0;

  m = (Material *) malloc(MAX_MATERIALS * sizeof(Material));

  if((f = fopen(filename, "r")) == 0) {
    fprintf(stderr, "could not open file '%s'\n", filename);
    return -1;
  }
  while(fgets(buf, sizeof(buf), f)) {
    switch(buf[0]) {
    case 'n':
      if(sscanf(buf, "newmtl %s ", namebuf) == 1) {
	iMaterial++;
	(m + iMaterial)->name = (char*) malloc(strlen(namebuf) + 1);
	sprintf((m + iMaterial)->name, "%s", namebuf);
	
	(m + iMaterial)->ambient[3] = 1.0;
	(m + iMaterial)->diffuse[3] = 1.0;
	(m + iMaterial)->specular[3] = 1.0;
      } else {
	fprintf(stderr, "warning: ignored line %d\n", iLine);
      }
      break;
    case 'K':
      if(iMaterial >= 0) {
	switch(buf[1]) {
	case 'a': sscanf(buf, "Ka %f %f %f",
			 (m + iMaterial)->ambient,
			 (m + iMaterial)->ambient + 1,
			 (m + iMaterial)->ambient + 2);
	break;
	case 'd': sscanf(buf, "Kd %f %f %f",
			 (m + iMaterial)->diffuse,
			 (m + iMaterial)->diffuse + 1,
			 (m + iMaterial)->diffuse + 2);
	break;
	case 's': sscanf(buf, "Ks %f %f %f",
			 (m + iMaterial)->specular,
			 (m + iMaterial)->specular + 1,
			 (m + iMaterial)->specular + 2);
	break;
	default: 
	  fprintf(stderr, "unknown light model at line %d\n", iLine);
	  break;
	}
      }
      break;
      /* ignore the rest... */
    }
    iLine++;
  }
  /* allocate the needed materials */
  /* copy the data */
  /* free the temporary memory */
  /* return number of materials */
  *(materials) = (Material*) malloc((iMaterial + 1) * sizeof(Material));
  memcpy(*materials, m, (iMaterial + 1) * sizeof(Material));
  free(m);
  fclose(f);
  return iMaterial + 1;
}
		       
	

    
	
	
      
			   
  

