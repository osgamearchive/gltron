#include "game/resource.h"
#include "base/nebu_util.h"
#include "video/nebu_font.h"
#include "video/nebu_texture2d.h"
#include "filesystem/nebu_filesystem.h"
#include "base/nebu_assert.h"
#include <string.h>

// TODO: move user type load functions somewhere else
#include "filesystem/path.h"
#include "video/model.h"

#include "base/nebu_debug_memory.h"

typedef struct {
	void *data; // the loaded & ready-to-use resource
	char *filename; // the resource location in the filesystem
	void *metadata;	// optional metadata information for the resource
	int metasize; // size of the metadata chunk
	int type; // resource type identifier
	int token; // the (unique) identifier for this resource
} ResourceToken;

typedef struct {
	int type;
	void* (*get)(char*,void*);
	void (*release)(void*);
} ResourceHandler;

static nebu_List *pTokenList = NULL;
static nebu_List handlerList;
static int nextToken = 1;

void* getTriMesh(char *filename, void *dummy)
{
	return gltron_Mesh_LoadFromFile(filename, TRI_MESH);
}

void* getQuadMesh(char *filename, void *dummy)
{
	return gltron_Mesh_LoadFromFile(filename, QUAD_MESH);
}

void* get2d(char *filename, void *dummy)
{
	char *path;
	path = nebu_FS_GetPath(PATH_ART, filename);
	if(path)
	{
		void *pData = nebu_2d_LoadPNG(path, 0);
		free(path);
		return pData;
	}
	else
	{
		fprintf(stderr, "failed to locate %s", filename);
		nebu_assert(0); exit(1); // installation corrupt
	}
}

void* getFont(char *filename, void *dummy)
{
	return nebu_Font_Load(filename, PATH_ART);
}

void* getTexture(char *filename, void *meta)
{
	char *path;
	path = nebu_FS_GetPath(PATH_ART, filename);
	if(path)
	{
		void *pData = nebu_Texture2D_Load(path, (nebu_Texture2D_meta*) meta);
		free(path);
		return pData;
	}
	else
	{
		fprintf(stderr, "failed to locate %s", filename);
		nebu_assert(0); exit(1); // installation corrupt
	}
}

void releaseMesh(void *data)
{
	gltron_Mesh_Free((gltron_Mesh*)data);
}

void resource_Init()
{
	resource_RegisterHandler(eRT_GLtronTriMesh, getTriMesh, releaseMesh);
	resource_RegisterHandler(eRT_GLtronQuadMesh, getQuadMesh, releaseMesh);
	resource_RegisterHandler(eRT_2d, get2d, NULL);
	resource_RegisterHandler(eRT_Font, getFont, NULL);
	resource_RegisterHandler(eRT_Texture, getTexture, NULL);
}

void resource_RegisterHandler(int type, void* (*get)(char*,void*), void (*release)(void*))
{
	ResourceHandler *pHandler;

	pHandler = (ResourceHandler*) malloc(sizeof(ResourceHandler));
	pHandler->type = type;
	pHandler->get = get;
	pHandler->release = release;
	nebu_List_AddTail(&handlerList, pHandler);
}

void resource_UnregisterHandler(int type)
{
	// FIXME: not implemented
}


static ResourceToken* findToken(int token)
{
	nebu_List *p;
	nebu_assert(token);
	nebu_assert(pTokenList);
	for(p = pTokenList; p->next; p = p->next)
	{
		ResourceToken* pToken = (ResourceToken*) p->data;
		if(pToken->token == token)
			return pToken;
	}
	return NULL;
}

static void release(ResourceToken *pToken)
{
	nebu_List *p;

	nebu_assert(pToken);
	if(!pToken->data)
		return;

	for(p = &handlerList; p->next; p = p->next)
	{
		ResourceHandler *pHandler = (ResourceHandler*) p->data;
		if(pToken->type == pHandler->type && pHandler->release)
		{
			pHandler->release(pToken->data);
			return;
		}
	}

	switch(pToken->type)
	{
	case eRT_2d:
		nebu_2d_Free((nebu_2d*)pToken->data);
		break;
	case eRT_Font:
		nebu_Font_Free((nebu_Font*)pToken->data);
		break;
	case eRT_Texture:
		nebu_Texture2D_Free((nebu_Texture2D*)pToken->data);
		break;
	default:
		nebu_assert(0);
		break;
	}
	pToken->data = NULL;
}

void resource_Release(int token)
{
	release( findToken(token) );
}

void resource_Free(int token)
{
	nebu_List *p;
	nebu_List *pPrev = NULL;
	nebu_assert(pTokenList);
	for(p = pTokenList; p->next; p = p->next)
	{
		ResourceToken *pToken = (ResourceToken*)p->data;
		if(pToken->token == token)
		{
			release( pToken );
			if(pToken->metadata)
				free(pToken->metadata);
			free(pToken->filename);
			free(pToken);
			nebu_List_RemoveAt(p, pPrev);
			// can't continue accessing a freed list item,
			// but we're done anyway
			break;
		}
		pPrev = p;
	}
}

void resource_ReleaseType(int type)
{
	nebu_List *p;
	if(!pTokenList)
		return;

	for(p = pTokenList; p->next; p = p->next)
	{
		if(((ResourceToken*)p->data)->type == type)
			release((ResourceToken*)p->data);
	}

}

void resource_ReleaseAll(void)
{
	nebu_List *p;
	if(!pTokenList)
		return;

	for(p = pTokenList; p->next; p = p->next)
	{
		release((ResourceToken*)p->data);
	}
}

void resource_FreeAll(void)
{
	nebu_List *p;
	if(!pTokenList)
		return;

	for(p = pTokenList; p->next; p = p->next)
	{
		ResourceToken *pResource = (ResourceToken*)p->data;
		nebu_assert(pResource);
		release(pResource);
		free(pResource->filename);
		if(pResource->metadata)
		{
			free(pResource->metadata);
		}
		free(pResource);
	}
	nebu_List_Free(pTokenList);
	pTokenList = NULL;
}

void* resource_Get(int token, int type)
{
	nebu_List *p;

	ResourceToken *pToken = findToken(token);
	nebu_assert(pToken);
	if(!pToken)
		return NULL;
	nebu_assert(pToken->type == type);
	if(pToken->data)
		return pToken->data;

	for(p = &handlerList; p->next; p = p->next)
	{
		ResourceHandler *pHandler = (ResourceHandler*) p->data;
		if(pToken->type == pHandler->type)
		{
			pToken->data = pHandler->get(pToken->filename, pToken->metadata);
			nebu_assert(pToken->data);
			return pToken->data;
		}
	}

	// no suitable registred handler has been found
	// here are some default handlers

	switch(pToken->type)
	{
	default:
		break;
	}
	nebu_assert(pToken->data);
	return pToken->data;
}

static ResourceToken* findIdenticalToken(char *filename, int type)
{
	nebu_List *p;
	if(!pTokenList)
		return NULL;
	for(p = pTokenList; p->next; p = p->next)
	{
		ResourceToken *pToken = (ResourceToken*) p->data;
		if(pToken->type == type && 
			strcmp(filename, pToken->filename) == 0)
			return pToken;
	}
	return NULL;
}

int resource_GetToken(char *filename, int type)
{
	return resource_GetTokenMeta(filename, type, NULL, 0);
}

int resource_GetTokenMeta(char *filename, int type, void *metadata, int metasize)
{
	ResourceToken *pToken;

	if(filename == NULL || *filename == 0)
		return 0;

	nebu_assert(!findIdenticalToken(filename, type));

	pToken = (ResourceToken*)malloc(sizeof(ResourceToken));
	pToken->data = NULL;
	if(!metadata)
	{
		pToken->metadata = NULL;
		pToken->metasize = 0;
	}
	else
	{
		nebu_assert(metasize);
		pToken->metadata = malloc(metasize);
		pToken->metasize = metasize;
		memcpy(pToken->metadata, metadata, metasize);
	}

	pToken->filename = (char*) malloc(strlen(filename) + 1);
	strcpy(pToken->filename, filename);
	pToken->type = type;
	pToken->token = nextToken;
	if(!pTokenList)
		pTokenList = nebu_List_Create();
	nebu_List_AddTail(pTokenList, pToken);
	
	nextToken++;
	return pToken->token;
}
