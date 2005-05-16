#include "game/resource.h"
#include "base/nebu_util.h"
#include "video/nebu_font.h"

#include <assert.h>
#include <string.h>

// TODO: move user type load functions somewhere else
#include "filesystem/path.h"
#include "video/model.h"

#include "base/nebu_debug_memory.h"

typedef struct {
	void *data;
	char *filename;
	int type;
	int token;
} Token;

static nebu_List *pTokenList = NULL;
static int nextToken = 1;

static Token* findToken(int token)
{
	nebu_List *p;
	assert(token);
	assert(pTokenList);
	for(p = pTokenList; p->next; p = p->next)
	{
		Token* pToken = (Token*) p->data;
		if(pToken->token == token)
			return pToken;
	}
	return NULL;
}

static void release(Token *pToken)
{
	assert(pToken);
	if(!pToken->data)
		return;

	switch(pToken->type)
	{
	case eRT_GLtronTriMesh:
	case eRT_GLtronQuadMesh:
		gltron_Mesh_Free((gltron_Mesh*)pToken->data);
		break;
	case eRT_2d:
		nebu_2d_Free((nebu_2d*)pToken->data);
		break;
	case eRT_Font:
		nebu_Font_Free((nebu_Font*)pToken->data);
		break;
	default:
		assert(0);
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
	assert(pTokenList);
	for(p = pTokenList; p->next; p = p->next)
	{
		Token *pToken = (Token*)p->data;
		if(pToken->token == token)
		{
			release( pToken );
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

void resource_ReleaseAll(void)
{
	nebu_List *p;
	if(!pTokenList)
		return;

	for(p = pTokenList; p->next; p = p->next)
	{
		release((Token*)p->data);
	}
}

void resource_FreeAll(void)
{
	nebu_List *p;
	if(!pTokenList)
		return;

	for(p = pTokenList; p->next; p = p->next)
	{
		release((Token*)p->data);
		free(((Token*)p->data)->filename);
		free(p->data);
	}
	nebu_List_Free(pTokenList);
	pTokenList = NULL;
}

void* resource_Get(int token, int type)
{
	Token *pToken = findToken(token);
	assert(pToken);
	if(!pToken)
		return NULL;
	assert(pToken->type == type);
	if(pToken->data)
		return pToken->data;

	switch(pToken->type)
	{
	case eRT_GLtronTriMesh:
		pToken->data = gltron_Mesh_LoadFromFile(pToken->filename, TRI_MESH);
		break;
	case eRT_GLtronQuadMesh:
		pToken->data = gltron_Mesh_LoadFromFile(pToken->filename, QUAD_MESH);
		break;
	case eRT_2d:
		pToken->data = nebu_2d_LoadPNG(pToken->filename, 0);
		break;
	case eRT_Font:
		pToken->data = nebu_Font_Load(pToken->filename, PATH_ART);
		break;
	default:
		assert(0);
		break;

		// custom load function
	}
	assert(pToken->data);
	return pToken->data;
}

Token* findIdenticalToken(char *filename, int type)
{
	nebu_List *p;
	if(!pTokenList)
		return NULL;
	for(p = pTokenList; p->next; p = p->next)
	{
		Token *pToken = (Token*) p->data;
		if(pToken->type == type && 
			strcmp(filename, pToken->filename) == 0)
			return pToken;
	}
	return NULL;
}

int resource_GetToken(char *filename, int type)
{
	Token *pToken;

	assert(!findIdenticalToken(filename, type));

	pToken = (Token*)malloc(sizeof(Token));
	pToken->data = NULL;
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
