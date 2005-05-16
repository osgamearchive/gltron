#ifndef RESOURCE_H
#define RESOURCE_H

#include "video/nebu_2d.h"

typedef enum EResourceType
{
	eRT_Surface,
	eRT_2d,
	eRT_Texture,
	eRT_Font,
	eRT_UserTypes
} EResourceType;

typedef enum EResourceUserTypes {
	eRT_GLtronTriMesh = eRT_UserTypes,
	eRT_GLtronQuadMesh
} EResourceUserTypes;

void resource_Release(int token);
void resource_ReleaseAll(void);
void resource_FreeAll(void);
void resource_Free(int token);
void* resource_Get(int token, int type);
int resource_GetToken(char *filename, int type);

#endif /* RESOURCE_H */