#include "video/nebu_scene.h"

#include <stdlib.h>

void nebu_Scene_AddMesh(nebu_Scene *pScene, nebu_Mesh* pMesh)
{
}

void nebu_Scene_AddObject(nebu_Scene *pScene, nebu_Shader *pShader, nebu_Mesh *pMesh)
{
}

nebu_Scene* nebu_Scene_Create(void)
{
	nebu_Scene *pScene = NULL;

	pScene = (nebu_Scene*) malloc(sizeof(nebu_Scene));
	if(!pScene) return NULL;

	// TODO: init other stuff
	return pScene;
}

void nebu_Scene_SetCamera(nebu_Scene *pScene, nebu_Camera *pCamera)
{
}

void nebu_Scene_Draw(nebu_Scene *pScene)
{
}

nebu_Shader* nebu_Shader_Create(void)
{
	nebu_Shader *pShader = NULL;

	pShader = (nebu_Shader*) malloc(sizeof(nebu_Shader));
	if(!pShader) return NULL;

	// TODO: init other stuff
	return pShader;
}
